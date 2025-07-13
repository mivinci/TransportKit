#pragma once
#include <queue>
#include <unordered_set>
#include <utility>

#include "KFC/Condvar.h"
#include "KFC/Mutex.h"
#include "KFC/String.h"
#include "KFC/Thread.h"
#include "KFC/Time.h"

namespace KFC {

enum RunLoopObserverEvent {
  kRunLoopObserverEventEntry = 1U << 0,
  kRunLoopObserverEventBeforeTimers = 1U << 1,
  kRunLoopObserverEventBeforeSources = 1U << 2,
  kRunLoopObserverEventBeforeBlocks = 1U << 3,
  kRunLoopObserverEventBeforeWaiting = 1U << 4,
  kRunLoopObserverEventAfterWaiting = 1U << 5,
  kRunLoopObserverEventExit = 1U << 6,
  kRunLoopObserverEventAll = 0xFF
};

enum RunLoopStatus {
  kRunLoopStatusFinished = 0,
  kRunLoopStatusTimeout = 1,
  kRunLoopStatusStopped = 2,
};

enum RunLoopTimerRepeat {
  kRunLoopTimerRepeatNever = 0,
  kRunLoopTimerRepeatAlways = UINT64_MAX,
};

class RunLoop final {
 public:
  using Func = std::function<void()>;
  explicit RunLoop() : m_shared({}), m_period(Duration::Second(1)), m_cycles(0), m_stopped(false) {}

  KFC_NODISCARD bool isStopped() const;
  KFC_NODISCARD Tick getCycles() const;
  KFC_NODISCARD Duration getNextTimeout(const Time &now);

  void reset();
  void stop();
  void wakeUp();

  RunLoopStatus runUntil(const Duration timeout) { return run(timeout, UINT64_MAX); }
  RunLoopStatus runCycles(const Tick cycles) { return run(Duration::FOREVER, cycles); }
  RunLoopStatus run(Duration timeout = Duration::FOREVER, Tick cycles = UINT64_MAX);

  class Observer {
   public:
    explicit Observer(const bool once = false,
                      const RunLoopObserverEvent activities = kRunLoopObserverEventAll)
        : m_activities(activities), m_once(once) {}
    virtual ~Observer() = default;
    virtual void onRunLoopObserverEvent(const RunLoop &, RunLoopObserverEvent) = 0;

   private:
    friend RunLoop;
    RunLoopObserverEvent m_activities;
    bool m_once;
  };

  class Event {
   public:
    explicit Event() : m_alive(kMagicLiveNum) {}
    ~Event() { m_alive = 0; }

   protected:
    KFC_NODISCARD KFC_CONSTEXPR_FUNCTION bool isAlive() const { return m_alive == kMagicLiveNum; }

   private:
    friend RunLoop;
    static constexpr unsigned kMagicLiveNum = 0xDEADC0DE;
    unsigned m_alive;
  };

  class Source : protected Event {
   public:
    explicit Source() : m_signaledTime(0) {}
    virtual ~Source() = default;
    virtual void onSchedule(const RunLoop &) = 0;
    virtual void onCancel(const RunLoop &) = 0;
    virtual void onPerform(const RunLoop &) = 0;
    KFC_NODISCARD uint64_t signaledTime() const;
    KFC_NODISCARD bool isSignaled() const { return signaledTime() != 0; }
    void signal();
    void clear();

   private:
    friend RunLoop;
    std::atomic<uint64_t> m_signaledTime;
  };

  class Timer : protected Event {
   public:
    explicit Timer(const Duration timeout, const Duration period = -1,
                   const uint64_t repeat = kRunLoopTimerRepeatNever, String name = "Timer")
        : m_deadline(Time::Now() + timeout),
          m_period(period),
          m_repeat(repeat),
          m_tick(0),
          m_name(std::move(name)) {}

    virtual ~Timer() = default;
    virtual void onTimeout(const RunLoop &) = 0;
    KFC_NODISCARD Tick ticks() const { return m_tick; }

   protected:
    friend RunLoop;

    Time m_deadline;
    Duration m_period;
    uint64_t m_repeat;
    Tick m_tick;
    String m_name;
  };

  void addSource(Source *);
  void removeSource(Source *);

  void addTimer(Timer *);
  void removeTimer(Timer *);

  void addObserver(Observer *);
  void removeObserver(Observer *);

  void performBlock(Func);

  static RunLoop &current();

 private:
  friend class WaitScope;

  struct CompareTimerPtr {
    bool operator()(const Timer *t1, const Timer *t2) const {
      return t2->m_deadline < t1->m_deadline;
    }
  };

  struct Shared {
    std::unordered_set<Observer *> m_observerSet;
    std::unordered_set<Source *> m_sourceSet;
    std::priority_queue<Timer *, std::vector<Timer *>, CompareTimerPtr> m_timerQueue;
    std::queue<Func> m_blockQueue;
  };

  bool wait(Duration timeout = Duration::FOREVER);
  void enterScope() const;
  void leaveScope() const;
  void doObservers(RunLoopObserverEvent);
  void doSources();
  void doTimers();
  void doBlocks();

  struct Metrics {
    uint64_t numSourcePerformed;
    uint64_t numTimerPerformed;
    uint64_t numBlockPerformed;
    Metrics() : numSourcePerformed(0), numTimerPerformed(0), numBlockPerformed(0) {}
    void reset() {
      numSourcePerformed = 0;
      numTimerPerformed = 0;
      numBlockPerformed = 0;
    }
  };

  Mutex<Shared> m_shared;
  Condvar m_condvar;
  Duration m_period;
  Tick m_cycles;
  std::atomic<bool> m_stopped;
  Metrics m_metrics;
};

#define KFC_CURRENT_RUNLOOP (KFC::RunLoop::current())

}  // namespace KFC
