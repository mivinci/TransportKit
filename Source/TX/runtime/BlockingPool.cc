#include "TX/runtime/BlockingPool.h"

#include "TX/Thread.h"

namespace TX {
void BlockingPool::SpawnTask(UnownedTask &&task) {
  auto shared = shared_.Lock();
  shared->queue.push(std::move(task));
  if (shared->num_idle_threads <= 0) {
    if (shared->num_threads < max_threads_) {
      int id = shared->num_threads;
      shared->threads[id] = Thread::Spawn([this, id]() { Run(id); });
      shared->num_threads++;
    }
  } else {
    cond_.NotifyOne();
  }
}

void BlockingPool::Run(int) {
  auto shared = shared_.Lock();
  while (true) {
    while (!shared->queue.empty()) {
      auto task = std::move(shared->queue.front());
      shared->queue.pop();
      Drop(shared);
      task.Run();
      shared = shared_.Lock();
    }

    if (shared->shutdown) {
      while (!shared->queue.empty()) {
        auto task = std::move(shared->queue.front());
        shared->queue.pop();
        Drop(shared);
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
  std::vector<Own<Thread>> threads;
  {
    auto shared = shared_.Lock();
    if (shared->shutdown) return;
    shared->shutdown = true;
    // We have to first release the lock and then notify and join all worker
    // threads before returning to the caller of shutdown.
    for (auto &t : shared->threads) {
      threads.push_back(std::move(t.second));
    }
  }
  cond_.NotifyAll();
}
}  // namespace TX
