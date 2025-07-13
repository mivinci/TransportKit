#include "KFC/ThreadPool.h"
#include "KFC/Memory.h"
#include "KFC/System.h"

KFC_NAMESPACE_BEG

ThreadPool::ThreadPool(const int maxNumThreads, const int minNumThreads, const int maxAge,
                       const int maxSleepSeconds)
    : m_guarded(genSafeWorkerThreadMaxNum(maxNumThreads), genSafeWorkerThreadMinNum(minNumThreads),
                genSafeWorkerThreadMaxAge(maxAge),
                genSafeWorkerThreadMaxSleepSeconds(maxSleepSeconds)) {}

ThreadPool::~ThreadPool() noexcept(false) { shutdown(); }

void ThreadPool::setWorkerThreadMinNum(const int num) {
  m_guarded.lock()->minNumThreads = genSafeWorkerThreadMinNum(num);
}

void ThreadPool::setWorkerThreadMaxNum(const int num) {
  m_guarded.lock()->maxNumThreads = genSafeWorkerThreadMaxNum(num);
}

void ThreadPool::setWorkerThreadMaxAge(const int age) {
  m_guarded.lock()->maxAge = genSafeWorkerThreadMaxAge(age);
}

void ThreadPool::setWorkerThreadMaxSleepSeconds(const int seconds) {
  m_guarded.lock()->maxSleepSeconds = genSafeWorkerThreadMaxSleepSeconds(seconds);
}

void ThreadPool::submitTask(Task &&task) {
  auto guarded = m_guarded.lock();
  enqueueTaskLocked(std::move(task), guarded);
  if (guarded->numIdleThreads <= 0) {
    if (guarded->numThreads < guarded->maxNumThreads) {
      int seq = genWorkerThreadSeqLocked(guarded);
      const std::string name = genWorkerThreadName(seq);
      guarded->threads.insert({seq, Thread::spawn([this, seq] { runWorker(seq); }, name)});
      guarded->numThreads++;
    }
  } else {
    m_condition.notifyOne();
  }
}

void ThreadPool::runWorker(const int seq) {
  Option<OwnThread> lastExitingThread;
  int age = 0;
  auto guarded = m_guarded.lock();

  while (true) {
    while (!isTaskQueueEmptyLocked(guarded)) {
      Task task = dequeueTaskLocked(guarded);
      KFC_GIVE_UP_GUARD(guarded);
      task.run();
      guarded = m_guarded.lock();
    }

    if (guarded->shutdown) {
      while (!isTaskQueueEmptyLocked(guarded)) {
        auto task = dequeueTaskLocked(guarded);
        KFC_GIVE_UP_GUARD(guarded);
        task.shutdown();
        guarded = m_guarded.lock();
      }
      break;
    }

    if (age > guarded->maxAge && guarded->numThreads > guarded->minNumThreads) {
      // Here it is the only chance we can take the ownership of the last exiting thread so that it
      // can be `join`ed when `runWorker` returns, before it is too late...
      lastExitingThread = std::move(guarded->lastExitingThread);
      guarded->lastExitingThread = std::move(guarded->threads[seq]);
      guarded->threads.erase(seq);
      guarded->numThreads--;
      guarded->freeWorkerSeqSet |= 1U << seq;
      break;
    }

    guarded->numIdleThreads++;
    const bool woken = m_condition.wait(guarded, Duration::fromSecond(guarded->maxSleepSeconds));
    guarded->numIdleThreads--;

    if (woken) {
      age = 0;
    } else {
      age++;
    }
  }
}

void ThreadPool::shutdown() {
  auto guarded = m_guarded.lock();
  if (guarded->shutdown) return;

  guarded->shutdown = true;
  m_condition.notifyAll();

  // Take the ownership of all threads
  auto lastExitingThread = std::move(guarded->lastExitingThread);
  auto threads = std::move(guarded->threads);

  // We must unlock before joining all threads
  KFC_GIVE_UP_GUARD(guarded);
}

void ThreadPool::enqueueTaskLocked(Task &&task, MutexGuard<Guarded> &guarded) {
  if (task.m_prior) {
    guarded->queue.push_front(task);
  } else {
    guarded->queue.push_back(task);
  }
}

ThreadPool::Task ThreadPool::dequeueTaskLocked(MutexGuard<Guarded> &guarded) {
  Task task = std::move(guarded->queue.front());
  guarded->queue.pop_front();
  return task;
}

int ThreadPool::genWorkerThreadSeqLocked(MutexGuard<Guarded> &guarded) {
  const int seq = __builtin_ctz(guarded->freeWorkerSeqSet);
  guarded->freeWorkerSeqSet &= ~(1U << seq);
  return seq;
}

bool ThreadPool::isTaskQueueEmptyLocked(MutexGuard<Guarded> &guarded) {
  return guarded->queue.empty();
}

std::string ThreadPool::genWorkerThreadName(const int seq) {
  char buf[32];
  snprintf(buf, sizeof(buf), "KFC-Worker-%d", seq);
  buf[sizeof(buf) - 1] = '\0';
  return buf;
}

int ThreadPool::genSafeWorkerThreadMaxNum(const int num) {
  const int numCPUs = getProcessorCoreCount();
  const int numMax = numCPUs > 0 ? std::min(numCPUs, kWorkerThreadMaxNum) : kWorkerThreadMaxNum;
  return num > 0 ? std::min(num, numMax) : numMax;
}

int ThreadPool::genSafeWorkerThreadMinNum(const int num) {
  return num > 0 ? num : kWorkerThreadMinNum;
}

int ThreadPool::genSafeWorkerThreadMaxAge(const int age) {
  return age > 0 ? age : kWorkerThreadMaxAge;
}

int ThreadPool::genSafeWorkerThreadMaxSleepSeconds(const int seconds) {
  return seconds > 0 ? seconds : kWorkerThreadMaxSleepSeconds;
}

KFC_NAMESPACE_END
