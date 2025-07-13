#include "KFC/RunLoop.h"

#include <unordered_map>
#include <utility>

#include "KFC/Mutex.h"
#include "KFC/Platform.h"
#include "KFC/Trace.h"

namespace KFC {

static thread_local RunLoop threadLocalRunLoop;

RunLoop &RunLoop::current() { return threadLocalRunLoop; }

RunLoopStatus RunLoop::run(const Duration timeout, Tick cycles) {
  KFC_CHECK(!m_stopped, "RunLoop is already running");
  bool isTimeout = false;
  Duration elapse = 0;
  m_stopped = false;

  doObservers(kRunLoopObserverEventEntry);

  while (cycles--) {
    if (isStopped()) return kRunLoopStatusStopped;
    KFC_TRACE_START(std::to_string(m_cycles));

    Time start = Time::Now();
    Duration nextTimeout = getNextTimeout(start);
    if (nextTimeout <= 0) {
      doObservers(kRunLoopObserverEventBeforeTimers);
      doTimers();
      continue;
    }

    doObservers(kRunLoopObserverEventBeforeSources);
    doSources();

    Duration loopTimeout = nextTimeout - Time::Since(start);
    if (loopTimeout <= 0) {
      doObservers(kRunLoopObserverEventBeforeTimers);
      doTimers();
      continue;
    }

    doObservers(kRunLoopObserverEventBeforeWaiting);
    isTimeout = wait(KFC_MIN(loopTimeout, m_period));
    doObservers(kRunLoopObserverEventAfterWaiting);

    if (isTimeout && loopTimeout <= m_period) {
      doObservers(kRunLoopObserverEventBeforeTimers);
      doTimers();
    }

    doObservers(kRunLoopObserverEventBeforeBlocks);
    doBlocks();

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

void RunLoop::addSource(Source *source) {
  auto guard = m_shared.lock();
  guard->m_sourceSet.insert(source);
  source->OnSchedule(*this);
}

void RunLoop::removeSource(Source *source) {
  auto guard = m_shared.lock();
  guard->m_sourceSet.erase(source);
  source->OnCancel(*this);
}

void RunLoop::addTimer(Timer *timer) {
  auto guard = m_shared.lock();
  guard->m_timerQueue.push(timer);
}

void RunLoop::removeTimer(Timer *timer) {
  auto _ = m_shared.lock();
  timer->m_dead = true;  // we can just do soft remove on std::priority_queue
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
    (*it)->OnRunLoopObserverEvent(*this, event);
    if ((*it)->m_once) {
      it = guard->m_observerSet.erase(it);
    }
  }
}

void RunLoop::doSources() {
  auto guard = m_shared.lock();
  for (const auto source : guard->m_sourceSet) {
    if (source && source->isSignaled()) {
      source->clear();
      source->OnPerform(*this);
      m_metrics.numSourcePerformed++;
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
  if (timer->m_dead || timer->m_period == 0) return;
  timer->OnTimeout(*this);
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

void RunLoop::Source::signal() {
  uint64_t expected = 0;
  m_signaledTime.compare_exchange_strong(expected, Time::Now().UnixNano(),
                                         std::memory_order_acq_rel, std::memory_order_relaxed);
}

bool RunLoop::isStopped() const { return m_stopped.load(std::memory_order_acquire); }

void RunLoop::Source::clear() { m_signaledTime.store(0, std::memory_order_release); }

uint64_t RunLoop::Source::signaledTime() const {
  return m_signaledTime.load(std::memory_order_acquire);
}

Tick RunLoop::getCycles() const { return m_cycles; }

Duration RunLoop::getNextTimeout(const Time &now) {
  int retries = 0;
  auto guard = m_shared.lock();

retry:
  if (guard->m_timerQueue.empty() || retries > 5) return Duration::FOREVER;
  const auto timer = guard->m_timerQueue.top();
  if (timer->m_dead || timer->m_period == 0) {
    guard->m_timerQueue.pop();
    retries++;
    goto retry;
  }
  return timer->m_deadline - now;
}

}  // namespace KFC
