#pragma once

#include <queue>
#include <unordered_map>

#include "TX/Condvar.h"
#include "TX/Function.h"
#include "TX/Mutex.h"
#include "TX/Own.h"
#include "TX/Thread.h"
#include "TX/runtime/Task.h"

namespace TX {

template <class F>
class BlockingTask final : public Task {
 public:
  using R = ReturnType<F>;
  explicit BlockingTask(F f) : f_(std::move(f)) {}
  void Run() override {
    if constexpr (std::is_void_v<R>) {
      f_();
    } else {
      R r = f_();
      // ?
    }
  }

 private:
  F f_;
};

class UnownedTask {
 public:
  explicit UnownedTask(const Ref<Task> &task, const bool mandatory = true)
      : task_(task), mandatory_(mandatory) {}
  void Run() { task_->Run(); }
  void Shutdown() {
    if (mandatory_) Run();
  }

 private:
  Ref<Task> task_;
  bool mandatory_;
};

class BlockingPool {
 public:
  explicit BlockingPool(const int max_threads) : max_threads_(max_threads) {}
  ~BlockingPool() { Shutdown(); }

  TX_DISALLOW_COPY(BlockingPool)
  void Shutdown();

  template <class F>
  Task::Handle<ReturnType<F>> Spawn(F f, const bool mandatory = true) {
    auto blocking_task = adoptRef(*new BlockingTask<F>(std::move(f)));
    SpawnTask(UnownedTask(blocking_task, mandatory));
    return Task::Handle<ReturnType<F>>(blocking_task);
  }

  struct Shared {
    std::queue<UnownedTask> queue;
    std::unordered_map<int, Own<Thread>> threads;
    int num_threads;
    int num_idle_threads;
    bool shutdown;
    explicit Shared() : num_threads(0), num_idle_threads(0), shutdown(false) {}
  };

 private:
  void Run(int);
  void SpawnTask(const UnownedTask &task);

 private:
  int max_threads_;
  Condvar cond_;
  Mutex<Shared> shared_;
};
}  // namespace TX
