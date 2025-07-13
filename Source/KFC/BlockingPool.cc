#include "KFC/BlockingPool.h"
#include "KFC/Thread.h"

namespace KFC {
void BlockingPool::spawnTask(const Task &task) {
  auto shared = m_shared.lock();
  shared->queue.push(task);
  if (shared->num_idle_threads <= 0) {
    if (shared->num_threads < m_maxThreads) {
      int id = shared->num_threads;
      shared->threads.insert(
          {id, Thread::spawn([this, id]() { runWorker(id); })});
      shared->num_threads++;
    }
  } else {
    m_condvar.notifyOne();
  }
}

void BlockingPool::runWorker(int) {
  auto shared = m_shared.lock();
  while (true) {
    while (!shared->queue.empty()) {
      Task task = shared->queue.front();
      shared->queue.pop();
      drop(shared);
      task.run();
      shared = m_shared.lock();
    }

    if (shared->shutdown) {
      while (!shared->queue.empty()) {
        auto task = shared->queue.front();
        shared->queue.pop();
        drop(shared);
        task.shutdown();
        shared = m_shared.lock();
      }
      return;
    }

    shared->num_idle_threads++;
    m_condvar.wait(shared);
    shared->num_idle_threads--;
  }
}

void BlockingPool::shutdown() {
  auto shared = m_shared.lock();
  if (shared->shutdown) return;
  shared->shutdown = true;
  std::vector<Own<Thread>> threads;
  for (auto &t : shared->threads) {
    threads.push_back(std::move(t.second));
  }
  drop(shared);
  m_condvar.notifyAll();
}
}  // namespace KFC
