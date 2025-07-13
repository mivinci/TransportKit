#include "TX/runtime/BlockingPool.h"

#include "TX/Thread.h"

namespace TX {
void BlockingPool::SpawnTask(const Task &task) {
  auto shared = shared_.Lock();
  shared->queue.push(task);
  if (shared->num_idle_threads <= 0) {
    if (shared->num_threads < max_threads_) {
      int id = shared->num_threads;
      shared->threads.insert(
          {id, Thread::Spawn([this, id]() { RunWorker(id); })});
      shared->num_threads++;
    }
  } else {
    cond_.NotifyOne();
  }
}

void BlockingPool::RunWorker(int) {
  auto shared = shared_.Lock();
  while (true) {
    while (!shared->queue.empty()) {
      Task task = shared->queue.front();
      shared->queue.pop();
      drop(shared);
      task.Run();
      shared = shared_.Lock();
    }

    if (shared->shutdown) {
      while (!shared->queue.empty()) {
        auto task = shared->queue.front();
        shared->queue.pop();
        drop(shared);
        task.Shutdown();
        shared = shared_.Lock();
      }
      return;
    }

    shared->num_idle_threads++;
    cond_.Wait(shared);
    shared->num_idle_threads--;
  }
}

void BlockingPool::Shutdown() {
  auto shared = shared_.Lock();
  if (shared->shutdown) return;
  shared->shutdown = true;
  std::vector<Own<Thread>> threads;
  for (auto &t : shared->threads) {
    threads.push_back(std::move(t.second));
  }
  drop(shared);
  cond_.NotifyAll();
}
}  // namespace TX
