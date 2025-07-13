#pragma once
#include <queue>
#include <unordered_set>
#include <utility>

#include "RunLoop.h"
#include "TX/Condvar.h"
#include "TX/Mutex.h"
#include "TX/Ref.h"
#include "TX/Thread.h"
#include "TX/Time.h"

namespace TX {
class RunLoop final : public AtomicRefCounted<RunLoop> {
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
             const std::string &scope_name = Scope::DEFAULT);

  void Stop();
  void Wakeup();

  class Observer {
   public:
    explicit Observer(RunLoop *run_loop, const bool once = false,
                      const Activity activities = Activity::All)
        : m_runLoop(run_loop), m_activities(activities), m_once(once) {}
    virtual ~Observer() = default;
    virtual void OnActivity(RunLoop &, Activity) = 0;

   private:
    RunLoop *m_runLoop;
    Activity m_activities;
    bool m_once;
  };

  class Scope;

  class Source {
   public:
    explicit Source() : m_signaledTime(0) {}
    virtual ~Source() = default;
    virtual void OnSchedule(const std::string &scope_name) = 0;
    virtual void OnCancel(const std::string &scope_name) = 0;
    virtual void OnPerform(const std::string &scope_name) = 0;
    TX_NODISCARD uint64_t SignaledTime() const;
    TX_NODISCARD bool IsSignaled() const { return SignaledTime() != 0; }
    void Signal();
    void Clear();

   private:
    std::atomic<uint64_t> m_signaledTime;
  };

  class Timer {
   public:
    enum Repeat {
      kTimerRepeatNever = 0,
      kTimerRepeatAlways = UINT64_MAX,
    };
    explicit Timer(const Duration timeout, const Duration period = -1,
                   const uint64_t repeat = kTimerRepeatNever,
                   std::string name = "Timer")
        : m_deadline(Time::Now() + timeout),
          m_period(period),
          m_repeat(repeat),
          m_tick(0),
          m_name(std::move(name)),
          m_dead(false) {}

    virtual ~Timer() = default;
    virtual void OnTimeout(const std::string &scope_name) = 0;
    TX_NODISCARD Tick GetTick() const { return m_tick; }

   private:
    friend RunLoop;

    Time m_deadline;
    Duration m_period;
    uint64_t m_repeat;
    Tick m_tick;
    std::string m_name;
    std::atomic<bool> m_dead;
  };

  class Scope final : public AtomicRefCounted<Scope> {
   public:
    explicit Scope(std::string name, RunLoop *run_loop)
        : m_name(std::move(name)), m_runLoop(run_loop) {}

   private:
    friend RunLoop;

    TX_NODISCARD Duration Timeout(const Time &now);
    static std::string DEFAULT;

    struct CompareTimerPtr {
      bool operator()(const Timer *t1, const Timer *t2) const {
        return t2->m_deadline < t1->m_deadline;
      }
    };

    struct Shared {
      bool m_stopped = false;
      std::unordered_set<Observer *> m_observerSet;
      std::unordered_set<Source *> m_sourceSet;
      std::priority_queue<Timer *, std::vector<Timer *>, CompareTimerPtr>
          m_timerQueue;
      std::queue<FnOnce> m_blockQueue;
    };

    Mutex<Shared> m_shared;
    std::string m_name;
    RunLoop *m_runLoop;
  };

  void AddSource(Source *source,
                 const std::string &scope_name = Scope::DEFAULT);
  void RemoveSource(Source *source,
                    const std::string &scope_name = Scope::DEFAULT);

  void AddTimer(Timer *timer, const std::string &scope_name = Scope::DEFAULT);
  void RemoveTimer(Timer *timer,
                   const std::string &scope_name = Scope::DEFAULT);

  void AddObserver(Observer *observer,
                   const std::string &scope_name = Scope::DEFAULT);
  void RemoveObserver(Observer *observer,
                      const std::string &scope_name = Scope::DEFAULT);

  void PerformBlock(FnOnce func,
                    const std::string &scope_name = Scope::DEFAULT);

  void SetPeriod(const Duration period) { m_period = period; }
  TX_NODISCARD uint64_t GetTick() const { return m_tick; }
  TX_NODISCARD bool IsInCurrentThread() const {
    return IsInThread(Thread::Current());
  }
  TX_NODISCARD bool IsInMainThread() const {
    return IsInThread(Thread::Main());
  }
  TX_NODISCARD bool IsInThread(const Thread::Id thread_id) const {
    return m_threadId == thread_id;
  }

  static void ClearGlobalContext();
  static Own<Thread> SpawnThread(const std::string &name = "TXRunLoop");
  static Ref<RunLoop> FromThread(const Thread::Id &id);
  static Ref<RunLoop> Current() { return FromThread(Thread::Current()); }
  static Ref<RunLoop> Main() { return FromThread(Thread::Main()); }

 private:
  struct Shared {
    std::unordered_map<std::string, RefPtr<Scope>> m_scopeMap;
    RefPtr<Scope> m_currentScope;
  };

  explicit RunLoop(const Thread::Id thread_id)
      : m_shared({}),
        m_threadId(thread_id),
        m_period(Duration::Second(1)),
        m_tick(0),
        m_stopped(false) {}

  static Ref<RunLoop> Create(const Thread::Id thread_id) {
    return adoptRef(*new RunLoop(thread_id));
  }

  Status RunInScope(RefPtr<Scope> &scope, Duration max_timeout,
                    uint64_t repeat);
  TX_NODISCARD RefPtr<Scope> GetScope(const std::string &name, bool create);
  TX_NODISCARD RefPtr<Scope> GetScopeLocked(const std::string &name,
                                            bool create,
                                            MutexGuard<Shared> &guard);
  TX_NODISCARD bool IsStopped() const;
  bool Wait(Duration timeout = Duration::FOREVER);
  void DoObservers(RefPtr<Scope> scope, Activity activity);
  void DoSources(RefPtr<Scope> scope);
  void DoTimers(RefPtr<Scope> scope);
  void DoBlocks(RefPtr<Scope> scope);

 private:
  Mutex<Shared> m_shared;
  Condvar m_condVar;
  Thread::Id m_threadId;
  Duration m_period;
  Tick m_tick;
  std::atomic<bool> m_stopped;
};
}  // namespace TX
