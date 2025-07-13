#include "KFC/RunLoop.h"

#include <utility>

#include "KFC/Mutex.h"
#include "KFC/Platform.h"
#include "KFC/Trace.h"

namespace KFC {

static __thread RunLoop *threadLocalRunLoop;

RunLoop &RunLoop::current() {
  RunLoop *runLoop = threadLocalRunLoop;
  KFC_CHECK(runLoop, "No RunLoop in current thread");
  return *runLoop;
}

void RunLoop::enterScope() const {
  KFC_CHECK(threadLocalRunLoop == nullptr, "This thread already has a RunLoop");
  threadLocalRunLoop = const_cast<RunLoop *>(this);
}

void RunLoop::leaveScope() const {
  KFC_CHECK(threadLocalRunLoop == this, "RunLoop leaves a different thread than it enters");
  threadLocalRunLoop = nullptr;
}

RunLoopStatus RunLoop::run(const Duration timeout, Tick cycles) {
  KFC_CHECK(!m_stopped, "RunLoop is already running");
  bool wokenUp = false;
  Duration elapse = 0;
  m_stopped = false;

  doObservers(kRunLoopObserverEventEntry);

  while (cycles--) {
    Time start = Time::Now();

    if (isStopped()) return kRunLoopStatusStopped;
    KFC_TRACE_START(std::to_string(m_cycles));

    doObservers(kRunLoopObserverEventBeforeBlocks);
    doBlocks();

    Duration nextTimeout = getNextTimeout(start);
    if (nextTimeout <= 0) {
      doObservers(kRunLoopObserverEventBeforeTimers);
      doTimers();
      continue;
    }

    doObservers(kRunLoopObserverEventBeforeWaiting);
    wokenUp = wait(KFC_MIN(nextTimeout, m_period));
    doObservers(kRunLoopObserverEventAfterWaiting);

    if (!wokenUp && nextTimeout <= m_period) {
      doObservers(kRunLoopObserverEventBeforeTimers);
      doTimers();
    }

    elapse += Time::Since(start);
    if (elapse >= timeout) {
      KFC_TRACE_END;
      return kRunLoopStatusTimeout;
    }
    m_cycles++;
    KFC_TRACE_END;
  }
  return kRunLoopStatusFinished;
}

void RunLoop::reset() {
  m_cycles = 0;
  m_stopped = false;
  m_metrics.reset();
}

void RunLoop::stop() {
  m_stopped.store(true);
  // RunLoop may be blocking on Wait, we have to wake it up.
  wakeUp();
}

void RunLoop::wakeUp() { m_condvar.notifyOne(); }

bool RunLoop::wait(const Duration timeout) {
  auto guard = m_shared.lock();
  return m_condvar.wait(guard, timeout);
}

void RunLoop::addTimer(Timer *timer) {
  auto guard = m_shared.lock();
  guard->m_timerQueue.push(timer);
}

void RunLoop::removeTimer(Timer *timer) {
  auto _ = m_shared.lock();
  timer->m_alive = 0;  // we can just do soft remove on std::priority_queue
}

void RunLoop::addObserver(Observer *observer) {
  auto guard = m_shared.lock();
  guard->m_observerSet.insert(observer);
}

void RunLoop::removeObserver(Observer *observer) {
  auto guard = m_shared.lock();
  guard->m_observerSet.erase(observer);
}

void RunLoop::performBlock(std::function<void()> func) {
  auto guard = m_shared.lock();
  guard->m_blockQueue.push(std::move(func));
}

void RunLoop::doObservers(const RunLoopObserverEvent event) {
  auto guard = m_shared.lock();
  for (auto it = guard->m_observerSet.begin(); it != guard->m_observerSet.end(); ++it) {
    (*it)->onRunLoopObserverEvent(*this, event);
    if ((*it)->m_once) {
      it = guard->m_observerSet.erase(it);
    }
  }
}

void RunLoop::doTimers() {
  auto guard = m_shared.lock();
  if (guard->m_timerQueue.empty()) return;
  Timer *timer = guard->m_timerQueue.top();
  guard->m_timerQueue.pop();
  drop(guard);
  KFC_ASSERT(timer);
  if (!timer->isAlive() || timer->m_period == 0) return;
  timer->onTimeout(*this);
  timer->m_tick++;
  m_metrics.numTimerPerformed++;
  if (timer->m_repeat == timer->m_tick - 1) return;
  if (timer->m_period > 0) {
    timer->m_deadline = Time::Now() + timer->m_period;
    guard = m_shared.lock();
    guard->m_timerQueue.push(timer);
    KFC_DEBUG("timer %s refreshed, tick/repeat: %lu/%lu", timer->m_name.c_str(), timer->m_tick,
              timer->m_repeat);
  }
}

void RunLoop::doBlocks() {
  auto guard = m_shared.lock();
  while (!guard->m_blockQueue.empty()) {
    const auto func = std::move(guard->m_blockQueue.front());
    guard->m_blockQueue.pop();
    func();
    m_metrics.numBlockPerformed++;
  }
}

bool RunLoop::isStopped() const { return m_stopped.load(std::memory_order_acquire); }

Tick RunLoop::getCycles() const { return m_cycles; }

Duration RunLoop::getNextTimeout(const Time &now) {
  int retries = 0;
  auto guard = m_shared.lock();

retry:
  if (guard->m_timerQueue.empty() || retries > 5) return Duration::FOREVER;
  const auto timer = guard->m_timerQueue.top();
  if (!timer->isAlive() || timer->m_period == 0) {
    guard->m_timerQueue.pop();
    retries++;
    goto retry;
  }
  return timer->m_deadline - now;
}

}  // namespace KFC
