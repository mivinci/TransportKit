#pragma once

#include <queue>
#include <unordered_map>

#include "TX/Condvar.h"
#include "TX/Mutex.h"
#include "TX/Own.h"
#include "TX/Thread.h"

namespace TX {
class BlockingPool {
 public:
  using Func = std::function<void()>;

  class Task {
   public:
    explicit Task(Func f, const bool mandatory = true)
        : m_func(std::move(f)), m_mandatory(mandatory) {}
    void Run() const { m_func(); }
    void Shutdown() const {
      if (m_mandatory) Run();
    }

   private:
    Func m_func;
    bool m_mandatory;
  };

  explicit BlockingPool(const int max_threads) : m_maxThreads(max_threads) {}
  ~BlockingPool() { Shutdown(); }

  TX_DISALLOW_COPY(BlockingPool)
  void Shutdown();

  void Spawn(Func f, const bool mandatory = true) {
    SpawnTask(Task(std::move(f), mandatory));
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
  void RunWorker(int);
  void SpawnTask(const Task &task);

 private:
  int m_maxThreads;
  Condvar m_condvar;
  Mutex<Shared> m_shared;
};
}  // namespace TX
