#pragma once
#include <queue>
#include <unordered_set>

#include "RunLoop.h"
#include "TX/Condvar.h"
#include "TX/Mutex.h"
#include "TX/Ref.h"
#include "TX/Thread.h"
#include "TX/Time.h"

namespace TX {
class RunLoop final : public RefCounted<RunLoop> {
 public:
  enum class Status { Finished, Timeout, Stopped };
  enum class Activity : uint8_t {
    Entry = 1U << 0,
    BeforeTimers = 1U << 1,
    BeforeSources = 1U << 2,
    BeforeBlocks = 1U << 3,
    BeforeWaiting = 1U << 4,
    AfterWaiting = 1U << 5,
    Exit = 1U << 6,
    All = 0xFF,
  };

  using FnOnce = std::function<void()>;  // TODO: real FnOnce

  Status Run(uint64_t repeat = UINT64_MAX, Duration timeout = Duration::FOREVER,
             StringView scope_name = Scope::Default);

  void Stop();
  void Wakeup();

  class Observer {
   public:
    explicit Observer(RunLoop *run_loop, const bool once = false,
                      const Activity activities = Activity::All)
        : run_loop_(run_loop), activities_(activities), once_(once) {}
    virtual ~Observer() = default;
    virtual void OnActivity(RunLoop &, Activity) = 0;

   private:
    RunLoop *run_loop_;
    Activity activities_;
    bool once_;
  };

  class Scope;

  class Source {
   public:
    virtual ~Source() = default;
    virtual void OnSchedule(RunLoop &, RefPtr<Scope> &) {}
    virtual void OnCancel(RunLoop &, RefPtr<Scope> &) {}
    virtual void OnPerform(RunLoop &, RefPtr<Scope> &) {}
    TX_NODISCARD uint64_t SignaledTime() const;
    TX_NODISCARD bool IsSignaled() const { return SignaledTime() != 0; }
    void Signal();
    void Clear();

   private:
    std::atomic<uint64_t> signaled_time_;
  };

  class Timer {
   public:
    explicit Timer(const Duration timeout, const Duration period = -1,
                   const uint64_t repeat = 0)
        : deadline_(Time::Now() + timeout),
          period_(period),
          repeat_(repeat),
          canceled_(false) {}
    virtual ~Timer() = default;
    virtual void OnTimeout(RunLoop &, RefPtr<Scope> &) {}

    bool operator<(const Timer &other) const {
      return deadline_ > other.deadline_;
    }

   private:
    friend RunLoop;
    void Cancel() { canceled_ = true; }

    Time deadline_;
    Duration period_;
    uint64_t repeat_;
    bool canceled_ : 1;
  };

  class Scope final : public RefCounted<Scope> {
   public:
    explicit Scope(const StringView &name, RunLoop *run_loop)
        : name_(name), run_loop_(run_loop) {}

    TX_NODISCARD Duration Timeout(const Time &now);
    static StringView Default;

   private:
    friend RunLoop;
    struct Shared {
      bool stopped = false;
      std::unordered_set<Observer *> observer_set_;
      std::unordered_set<Source *> source_set_;
      std::priority_queue<Timer *> timer_heap_;
      std::queue<FnOnce> block_queue_;
    };
    StringView name_;
    Mutex<Shared> shared_;
    RunLoop *run_loop_;
  };

  void AddSource(Source *source, const StringView &scope_name = Scope::Default);
  void RemoveSource(Source *source,
                    const StringView &scope_name = Scope::Default);

  void AddTimer(Timer *timer, const StringView &scope_name = Scope::Default);
  void RemoveTimer(Timer *timer, const StringView &scope_name = Scope::Default);

  void AddObserver(Observer *observer,
                   const StringView &scope_name = Scope::Default);
  void RemoveObserver(Observer *observer,
                      const StringView &scope_name = Scope::Default);

  void PerformBlock(std::function<void()> func,
                    const StringView &scope_name = Scope::Default);

  void SetPeriod(const Duration period) { period_ = period; }
  TX_NODISCARD uint64_t GetTick() const { return tick_; }
  TX_NODISCARD bool IsInCurrentThread() const {
    return IsInThread(Thread::Current());
  }
  TX_NODISCARD bool IsInMainThread() const {
    return IsInThread(Thread::Main());
  }
  TX_NODISCARD bool IsInThread(const Thread::Id thread_id) const {
    return thread_id_ == thread_id;
  }

  static Ref<RunLoop> Current() { return FromThreadId(Thread::Current()); }
  static Ref<RunLoop> Main() { return FromThreadId(Thread::Main()); }
  static void ClearGlobalContext();

 private:
  explicit RunLoop()
      : thread_id_(Thread::Current()), period_(Duration::Second(1)), tick_(0) {}

  struct Shared {
    std::unordered_map<StringView, RefPtr<Scope>> scope_map_;
    RefPtr<Scope> current_scope_;
  };

  static Ref<RunLoop> Create() { return adoptRef(*new RunLoop); }
  static Ref<RunLoop> FromThreadId(const Thread::Id &id);
  Status Schedule(RefPtr<Scope> &scope, Duration max_timeout, uint64_t repeat);
  TX_NODISCARD RefPtr<Scope> GetScope(const StringView &name, bool create);
  TX_NODISCARD RefPtr<Scope> GetScopeLocked(const StringView &name, bool create,
                                            MutexGuard<Shared> &guard);
  TX_NODISCARD bool IsStopped() const;
  bool Wait(Duration timeout = Duration::FOREVER);
  void DoObservers(RefPtr<Scope> scope, Activity activity);
  void DoSources(RefPtr<Scope> scope);
  void DoTimers(RefPtr<Scope> scope);
  void DoBlocks(RefPtr<Scope> scope);

 private:
  Mutex<Shared> shared_;
  Condvar cond_;
  Thread::Id thread_id_;
  Duration period_;
  uint64_t tick_;
  std::atomic<bool> stopped_;
};
}  // namespace TX
