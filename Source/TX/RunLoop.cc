#include "TX/RunLoop.h"

#include <unordered_map>
#include <utility>

#include "TX/Mutex.h"
#include "TX/Platform.h"
#include "TX/Trace.h"

namespace TX {
struct RunLoopGlobalContext {
  std::unordered_map<Thread::Id, Ref<RunLoop>> run_loop_map_;

  void Clear() { run_loop_map_.clear(); }
};

static Mutex<RunLoopGlobalContext> runLoopGlobalContext;

Own<Thread> RunLoop::SpawnThread(const std::string &name) {
  return Thread::Spawn([] { Current()->Run(); }, name);
}

Ref<RunLoop> RunLoop::FromThread(const Thread::Id &id) {
  auto global_context = runLoopGlobalContext.Lock();
  const auto it = global_context->run_loop_map_.find(id);
  if (it != global_context->run_loop_map_.end()) return it->second;
  Ref<RunLoop> run_loop = Create(id);
  global_context->run_loop_map_.insert({id, run_loop});
  return run_loop;
}

RefPtr<RunLoop::Scope> RunLoop::GetScopeLocked(const std::string &name,
                                               const bool create,
                                               MutexGuard<Shared> &guard) {
  const auto it = guard->m_scopeMap.find(name);
  if (it != guard->m_scopeMap.end()) return it->second;
  if (!create) return nullptr;
  auto scope = adoptRef(new Scope(name, this));
  guard->m_scopeMap.insert({name, scope});
  return scope;
}

RefPtr<RunLoop::Scope> RunLoop::GetScope(const std::string &name,
                                         const bool create) {
  auto shared = m_shared.Lock();
  return GetScopeLocked(name, create, shared);
}

RunLoop::Status RunLoop::Run(const uint64_t repeat, const Duration timeout,
                             const std::string &scope_name) {
  TX_ASSERT(m_threadId == Thread::Current(),
            "RunLoop must be running on the thread it is created.");
  auto guard = m_shared.Lock();
  RefPtr<Scope> scope = GetScopeLocked(scope_name, true, guard);
  const RefPtr<Scope> previous_scope = guard->m_currentScope;
  guard->m_currentScope = scope;
  drop(guard);
  const Status status = RunInScope(scope, timeout, repeat);
  guard = m_shared.Lock();
  guard->m_currentScope = previous_scope;
  return status;
}

RunLoop::Status RunLoop::RunInScope(RefPtr<Scope> &scope,
                                    const Duration max_timeout,
                                    uint64_t repeat) {
  {
    auto scope_guard = scope->m_shared.Lock();
    if (scope_guard->m_stopped) return Status::Stopped;
  }

  bool timeout = false;
  Duration elapse_total = 0;
  m_stopped = false;

  DoObservers(scope, Activity::Entry);

  do {
    if (IsStopped()) return Status::Stopped;
    TX_TRACE_START(std::to_string(m_tick));

    Time start = Time::Now();
    Duration scope_timeout = scope->Timeout(start);
    if (scope_timeout <= 0) {
      DoObservers(scope, Activity::BeforeTimers);
      DoTimers(scope);
      continue;
    }

    DoObservers(scope, Activity::BeforeSources);
    DoSources(scope);

    Duration loop_timeout = scope_timeout - Time::Since(start);
    if (loop_timeout <= 0) {
      DoObservers(scope, Activity::BeforeTimers);
      DoTimers(scope);
      continue;
    }

    DoObservers(scope, Activity::BeforeWaiting);
    timeout = Wait(TX_MIN(loop_timeout, m_period));
    DoObservers(scope, Activity::AfterWaiting);

    if (timeout && loop_timeout <= m_period) {
      DoObservers(scope, Activity::BeforeTimers);
      DoTimers(scope);
    }

    DoObservers(scope, Activity::BeforeBlocks);
    DoBlocks(scope);

    elapse_total += Time::Since(start);
    if (elapse_total >= max_timeout) {
      TX_TRACE_END;
      return Status::Timeout;
    }
    m_tick++;
    TX_TRACE_END;
  } while (repeat--);
  return Status::Finished;
}

void RunLoop::Stop() {
  m_stopped.store(true);
  // RunLoop may be blocking on Wait, we have to wake it up.
  Wakeup();
}

void RunLoop::Wakeup() { m_condVar.NotifyOne(); }

bool RunLoop::Wait(const Duration timeout) {
  auto guard = m_shared.Lock();
  return m_condVar.Wait(guard, timeout);
}

void RunLoop::AddSource(Source *source, const std::string &scope_name) {
  auto guard = m_shared.Lock();
  RefPtr<Scope> scope = GetScopeLocked(scope_name, true, guard);
  drop(guard);
  TX_ASSERT(!!scope, "GetModeLocked failed to create a new RunLoop scope");
  scope->m_shared.Lock()->m_sourceSet.insert(source);
  source->OnSchedule(scope->m_name);
}

void RunLoop::RemoveSource(Source *source, const std::string &scope_name) {
  auto guard = m_shared.Lock();
  RefPtr<Scope> scope = GetScopeLocked(scope_name, false, guard);
  drop(guard);
  if (!scope) return;
  scope->m_shared.Lock()->m_sourceSet.erase(source);
  source->OnCancel(scope->m_name);
}

void RunLoop::AddTimer(Timer *timer, const std::string &scope_name) {
  auto guard = m_shared.Lock();
  RefPtr<Scope> scope = GetScopeLocked(scope_name, true, guard);
  drop(guard);
  TX_ASSERT(!!scope, "GetScopeLocked failed to create a new RunLoop scope");
  scope->m_shared.Lock()->m_timerQueue.push(timer);
}

void RunLoop::RemoveTimer(Timer *timer, const std::string &) {
  auto _ = m_shared.Lock();
  timer->m_dead = true;  // we can just do soft remove on std::priority_queue
}

void RunLoop::AddObserver(Observer *observer, const std::string &scope_name) {
  auto guard = m_shared.Lock();
  RefPtr<Scope> scope = GetScopeLocked(scope_name, true, guard);
  drop(guard);
  TX_ASSERT(!!scope, "GetScopeLocked failed to create a new RunLoop scope");
  scope->m_shared.Lock()->m_observerSet.insert(observer);
}

void RunLoop::RemoveObserver(Observer *observer,
                             const std::string &scope_name) {
  auto guard = m_shared.Lock();
  RefPtr<Scope> scope = GetScopeLocked(scope_name, false, guard);
  drop(guard);
  if (!scope) return;
  scope->m_shared.Lock()->m_observerSet.erase(observer);
}

void RunLoop::PerformBlock(std::function<void()> func,
                           const std::string &scope_name) {
  auto guard = m_shared.Lock();
  RefPtr<Scope> scope = GetScopeLocked(scope_name, true, guard);
  drop(guard);
  TX_ASSERT(!!scope, "GetModeLocked failed to create a new RunLoop scope");
  scope->m_shared.Lock()->m_blockQueue.push(std::move(func));
}

void RunLoop::DoObservers(RefPtr<Scope> scope, const Activity activity) {
  auto scope_guard = scope->m_shared.Lock();
  for (const auto it : scope_guard->m_observerSet) {
    it->OnActivity(*this, activity);
  }
}

void RunLoop::DoSources(RefPtr<Scope> scope) {
  auto scope_guard = scope->m_shared.Lock();
  for (const auto source : scope_guard->m_sourceSet) {
    if (source && source->IsSignaled()) {
      source->Clear();
      source->OnPerform(scope->m_name);
    }
  }
}

void RunLoop::DoTimers(RefPtr<Scope> scope) {
  auto scope_guard = scope->m_shared.Lock();
  if (scope_guard->m_timerQueue.empty()) return;
  Timer *timer = scope_guard->m_timerQueue.top();
  scope_guard->m_timerQueue.pop();
  drop(scope_guard);
  TX_ASSERT(timer);
  if (timer->m_dead || timer->m_period == 0) return;
  timer->OnTimeout(scope->m_name);
  timer->m_tick++;
  if (timer->m_repeat == timer->m_tick - 1) return;
  if (timer->m_period > 0) {
    timer->m_deadline = Time::Now() + timer->m_period;
    scope_guard = scope->m_shared.Lock();
    scope_guard->m_timerQueue.push(timer);
    TX_DEBUG("timer %s refreshed, tick/repeat: %lu/%lu", timer->m_name.c_str(),
             timer->m_tick, timer->m_repeat);
  }
}

void RunLoop::DoBlocks(RefPtr<Scope> scope) {
  auto scope_guard = scope->m_shared.Lock();
  while (!scope_guard->m_blockQueue.empty()) {
    const auto func = std::move(scope_guard->m_blockQueue.front());
    scope_guard->m_blockQueue.pop();
    func();
  }
}

void RunLoop::Source::Signal() {
  uint64_t expected = 0;
  m_signaledTime.compare_exchange_strong(expected, Time::Now().UnixNano(),
                                         std::memory_order_acq_rel,
                                         std::memory_order_relaxed);
}

bool RunLoop::IsStopped() const {
  return m_stopped.load(std::memory_order_acquire);
}

void RunLoop::Source::Clear() {
  m_signaledTime.store(0, std::memory_order_release);
}

uint64_t RunLoop::Source::SignaledTime() const {
  return m_signaledTime.load(std::memory_order_acquire);
}

Duration RunLoop::Scope::Timeout(const Time &now) {
  int retries = 0;
  auto guard = m_shared.Lock();
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

void RunLoop::ClearGlobalContext() { runLoopGlobalContext.Lock()->Clear(); }

std::string RunLoop::Scope::DEFAULT = "default";

}  // namespace TX
