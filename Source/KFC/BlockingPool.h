#pragma once

#include <queue>
#include <unordered_map>

#include "KFC/Condvar.h"
#include "KFC/Mutex.h"
#include "KFC/Own.h"
#include "KFC/Thread.h"

namespace KFC {
class BlockingPool {
 public:
  using Func = std::function<void()>;

  class Task {
   public:
    explicit Task(Func f, const bool mandatory = true)
        : m_func(std::move(f)), m_mandatory(mandatory) {}
    void run() const { m_func(); }
    void shutdown() const {
      if (m_mandatory) run();
    }

   private:
    Func m_func;
    bool m_mandatory;
  };

  explicit BlockingPool(const int max_threads) : m_maxThreads(max_threads) {}
  ~BlockingPool() { shutdown(); }

  KFC_DISALLOW_COPY(BlockingPool)
  void shutdown();

  void spawn(Func f, const bool mandatory = true) {
    spawnTask(Task(std::move(f), mandatory));
  }

  struct Shared {
    std::queue<Task> queue;
    std::unordered_map<int, Own<Thread>> threads;
    int num_threads;
    int num_idle_threads;
    bool shutdown;
    explicit Shared() : num_threads(0), num_idle_threads(0), shutdown(false) {}
  };

 private:
  void runWorker(int);
  void spawnTask(const Task &task);

  int m_maxThreads;
  Condvar m_condvar;
  Mutex<Shared> m_shared;
};
}  // namespace KFC
