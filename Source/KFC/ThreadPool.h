#pragma once

#include "KFC/Condvar.h"
#include "KFC/CopyMove.h"
#include "KFC/Option.h"
#include "KFC/Preclude.h"
#include "KFC/Thread.h"

#include <functional>
#include <queue>
#include <unordered_map>
#include <unordered_set>

KFC_NAMESPACE_BEG

constexpr int kWorkerThreadMaxNumAuto = -1;
constexpr int kWorkerThreadMaxNum = 5;
constexpr int kWorkerThreadMinNum = 1;
constexpr int kWorkerThreadMaxAge = 5;
constexpr int kWorkerThreadMaxSleepSeconds = 60;

class ThreadPool {
public:
  KFC_DISALLOW_COPY(ThreadPool)
  explicit ThreadPool(int maxNumThreads = kWorkerThreadMaxNumAuto,
                      int minNumThreads = kWorkerThreadMinNum, int maxAge = kWorkerThreadMaxAge,
                      int maxSleepSeconds = kWorkerThreadMaxSleepSeconds);
  ~ThreadPool() noexcept(false);

  // Spawn a task that will be executed by a worker thread. `mandatory` means that the task
  // will be executed even when the pool is shutting down. `prior` means that the task
  // will be executed before other tasks.
  template <class Func>
  void submit(Func &&f, const bool mandatory = true, const bool prior = false) {
    submitTask(Task(std::forward<Func>(f), mandatory, prior));
  }

  void shutdown();

  void setWorkerThreadMinNum(int num);
  void setWorkerThreadMaxNum(int num);
  void setWorkerThreadMaxAge(int age);
  void setWorkerThreadMaxSleepSeconds(int seconds);

private:
  class Task {
  private:
    using Func = std::function<void()>;
    explicit Task() : m_func(nullptr), m_mandatory(true), m_prior(false) {}
    explicit Task(Func &&f, const bool mandatory = true, const bool prior = false)
        : m_func(std::move(f)), m_mandatory(mandatory), m_prior(prior) {}

    void run() const { m_func(); }
    void shutdown() const {
      if (m_mandatory) run();
    }

    Func m_func;
    bool m_mandatory;
    bool m_prior;

    friend class ThreadPool;
  };

  struct Guarded {
    std::deque<Task> queue;
    std::unordered_map<int, OwnThread> threads;
    Option<OwnThread> lastExitingThread;

    int numThreads;
    int numIdleThreads;
    uint32_t freeWorkerSeqSet;
    bool shutdown;

    int minNumThreads;
    int maxNumThreads;
    int maxAge;
    int maxSleepSeconds;

    Guarded(const int maxNumThreads, const int minNumThreads, const int maxAge,
            const int maxSleepSeconds)
        : numThreads(0), numIdleThreads(0), freeWorkerSeqSet((1U << maxNumThreads) - 1),
          shutdown(false), minNumThreads(minNumThreads), maxNumThreads(maxNumThreads),
          maxAge(maxAge), maxSleepSeconds(maxSleepSeconds) {}
  };

  void runWorker(int seq);
  void submitTask(Task &&task);

  /* Some helper methods */

  static void enqueueTaskLocked(Task &&task, MutexGuard<Guarded> &guarded);
  static Task dequeueTaskLocked(MutexGuard<Guarded> &guarded);
  static bool isTaskQueueEmptyLocked(MutexGuard<Guarded> &guarded);
  static int genWorkerThreadSeqLocked(MutexGuard<Guarded> &guarded);
  static std::string genWorkerThreadName(int seq);
  static int genSafeWorkerThreadMinNum(int num);
  static int genSafeWorkerThreadMaxNum(int num);
  static int genSafeWorkerThreadMaxAge(int age);
  static int genSafeWorkerThreadMaxSleepSeconds(int seconds);

  Condvar m_condvar;
  Mutex<Guarded> m_guarded;
};

KFC_NAMESPACE_END
