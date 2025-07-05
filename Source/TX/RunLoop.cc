#include "TX/RunLoop.h"

#include <unordered_map>
#include <utility>

#include "TX/Mutex.h"
#include "TX/Trace.h"

namespace TX {
struct RunLoopGlobalContext {
  std::unordered_map<Thread::Id, Ref<RunLoop>> run_loop_map_;

  void Clear() { run_loop_map_.clear(); }
};

static Mutex<RunLoopGlobalContext> runLoopGlobalContext;

Own<Thread> RunLoop::SpawnThread(const String &name) {
  return Thread::Spawn([] { Current()->Run(); }, name);
}

Ref<RunLoop> RunLoop::FromThread(const Thread::Id &id) {
  auto global_context = runLoopGlobalContext.Lock();
  if (const auto it = global_context->run_loop_map_.find(id);
      it != global_context->run_loop_map_.end())
    return it->second;
  Ref<RunLoop> run_loop = Create(id);
  global_context->run_loop_map_.insert({id, run_loop});
  return run_loop;
}

RefPtr<RunLoop::Scope> RunLoop::GetScopeLocked(const StringView &name,
                                               const bool create,
                                               MutexGuard<Shared> &guard) {
  if (const auto it = guard->scope_map_.find(name);
      it != guard->scope_map_.end())
    return it->second;
  if (!create) return nullptr;
  auto scope = new Scope(name, this);
  guard->scope_map_.emplace(name, scope);
  return scope;
}

RefPtr<RunLoop::Scope> RunLoop::GetScope(const StringView &name,
                                         const bool create) {
  auto shared = shared_.Lock();
  return GetScopeLocked(name, create, shared);
}

RunLoop::Status RunLoop::Run(const uint64_t repeat, const Duration timeout,
                             const StringView scope_name) {
  TX_ASSERT(thread_id_ == Thread::Current(),
            "RunLoop must be running on the thread it is created.");
  auto guard = shared_.Lock();
  RefPtr<Scope> scope = GetScopeLocked(scope_name, false, guard);
  if (!scope) return Status::Finished;
  const RefPtr<Scope> previous_scope = guard->current_scope_;
  guard->current_scope_ = scope;
  drop(guard);
  const Status status = Schedule(scope, timeout, repeat);
  guard = shared_.Lock();
  guard->current_scope_ = previous_scope;
  return status;
}

RunLoop::Status RunLoop::Schedule(RefPtr<Scope> &scope,
                                  const Duration max_timeout, uint64_t repeat) {
  {
    auto scope_guard = scope->shared_.Lock();
    if (scope_guard->stopped) return Status::Stopped;
  }

  bool timeout = false;
  Duration elapse_total = 0;
  stopped_ = false;

  DoObservers(scope, Activity::Entry);

  do {
    if (IsStopped()) return Status::Stopped;
    TX_TRACE_START(std::to_string(tick_));

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
    timeout = Wait(std::min(loop_timeout, period_));
    DoObservers(scope, Activity::AfterWaiting);

    if (timeout && loop_timeout <= period_) {
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
    tick_++;
    TX_TRACE_END;
  } while (repeat--);
  return Status::Finished;
}

void RunLoop::Stop() { stopped_.store(true); }

void RunLoop::Wakeup() { cond_.NotifyOne(); }

bool RunLoop::Wait(const Duration timeout) {
  auto guard = shared_.Lock();
  return cond_.Wait(guard, timeout);
}

void RunLoop::AddSource(Source *source, const StringView &scope_name) {
  auto guard = shared_.Lock();
  RefPtr<Scope> scope = GetScopeLocked(scope_name, true, guard);
  TX_ASSERT(!!scope, "GetModeLocked failed to create a new RunLoop scope");
  scope->shared_.Lock()->source_set_.insert(source);
  source->OnSchedule(*this, scope);
}

void RunLoop::RemoveSource(Source *source, const StringView &scope_name) {
  auto guard = shared_.Lock();
  RefPtr<Scope> scope = GetScopeLocked(scope_name, false, guard);
  if (!scope) return;
  scope->shared_.Lock()->source_set_.erase(source);
  source->OnCancel(*this, scope);
}

void RunLoop::AddTimer(Timer *timer, const StringView &scope_name) {
  auto guard = shared_.Lock();
  RefPtr<Scope> scope = GetScopeLocked(scope_name, true, guard);
  TX_ASSERT(!!scope, "GetModeLocked failed to create a new RunLoop scope");
  scope->shared_.Lock()->timer_heap_.push(timer);
}

void RunLoop::RemoveTimer(Timer *timer, const StringView &) {
  timer->Cancel();  // we can just do soft remove on std::priority_queue
}

void RunLoop::AddObserver(Observer *observer, const StringView &scope_name) {
  auto guard = shared_.Lock();
  RefPtr<Scope> scope = GetScopeLocked(scope_name, true, guard);
  TX_ASSERT(!!scope, "GetModeLocked failed to create a new RunLoop scope");
  scope->shared_.Lock()->observer_set_.insert(observer);
}

void RunLoop::RemoveObserver(Observer *observer, const StringView &scope_name) {
  auto guard = shared_.Lock();
  RefPtr<Scope> scope = GetScopeLocked(scope_name, false, guard);
  if (!scope) return;
  scope->shared_.Lock()->observer_set_.erase(observer);
}

void RunLoop::PerformBlock(std::function<void()> func,
                           const StringView &scope_name) {
  auto guard = shared_.Lock();
  RefPtr<Scope> scope = GetScopeLocked(scope_name, true, guard);
  TX_ASSERT(!!scope, "GetModeLocked failed to create a new RunLoop scope");
  scope->shared_.Lock()->block_queue_.push(std::move(func));
}

void RunLoop::DoObservers(RefPtr<Scope> scope, const Activity activity) {
  auto scope_guard = scope->shared_.Lock();
  for (const auto it : scope_guard->observer_set_) {
    it->OnActivity(*this, activity);
  }
}

void RunLoop::DoSources(RefPtr<Scope> scope) {
  auto scope_guard = scope->shared_.Lock();
  for (const auto source : scope_guard->source_set_) {
    if (source && source->IsSignaled()) {
      source->Clear();
      source->OnPerform(*this, scope);
    }
  }
}

void RunLoop::DoTimers(RefPtr<Scope> scope) {
  auto scope_guard = scope->shared_.Lock();
  if (scope_guard->timer_heap_.empty()) return;
  Timer *timer = scope_guard->timer_heap_.top();
  scope_guard->timer_heap_.pop();
  TX_ASSERT(timer);
  timer->OnTimeout(*this, scope);
  if (timer->repeat_-- <= 0) return;
  if (timer->period_ > 0) {
    timer->deadline_ = Time::After(timer->period_);
    scope_guard->timer_heap_.push(timer);
    TX_DEBUG("timer refreshed, repeat: %d", timer->repeat_);
  }
}

void RunLoop::DoBlocks(RefPtr<Scope> scope) {
  auto scope_guard = scope->shared_.Lock();
  while (!scope_guard->block_queue_.empty()) {
    const auto func = std::move(scope_guard->block_queue_.front());
    scope_guard->block_queue_.pop();
    func();
  }
}

void RunLoop::Source::Signal() {
  uint64_t expected = 0;
  signaled_time_.compare_exchange_strong(expected, Time::Now().UnixNano(),
                                         std::memory_order_acq_rel,
                                         std::memory_order_relaxed);
}

bool RunLoop::IsStopped() const {
  return stopped_.load(std::memory_order_acquire);
}

void RunLoop::Source::Clear() {
  signaled_time_.store(0, std::memory_order_release);
}

uint64_t RunLoop::Source::SignaledTime() const {
  return signaled_time_.load(std::memory_order_acquire);
}

Duration RunLoop::Scope::Timeout(const Time &now) {
  auto guard = shared_.Lock();
  if (guard->timer_heap_.empty()) return Duration::FOREVER;
  return guard->timer_heap_.top()->deadline_ - now;
}

void RunLoop::ClearGlobalContext() { runLoopGlobalContext.Lock()->Clear(); }

StringView RunLoop::Scope::Default = "default";

}  // namespace TX