#pragma once
#include "KFC/Condition.h"
#include "KFC/Mutex.h"

KFC_NAMESPACE_BEG

// WaitGroup is a synchronization primitive that allows multiple threads to wait for a
// collection of operations to finish.
//
// Example:
//   WaitGroup wg;
//   wg.add(1);
//   Thread t([&] {
//     // do something
//     wg.done();
//   });
//   wg.wait();
//
class WaitGroup {
public:
  explicit WaitGroup() : m_count(0) {}
  explicit WaitGroup(const int n) : m_count(0) { add(n); }

  // Returns the number of pending operations.
  int get() { return *m_count.lock(); }

  // Adds `n` pending operations.
  void add(const int n) { *m_count.lock() += n; }

  // Marks one pending operation as done.
  void done() {
    (*m_count.lock())--;
    m_condition.notifyOne();
  }

  // Waits for all pending operations to be done.
  void wait(const Duration &timeout = Duration::FOREVER) {
    auto count_locked = m_count.lock();
    while (*count_locked > 0) {
      m_condition.wait(count_locked, timeout);
    }
  }

private:
  Mutex<int> m_count;
  Condition m_condition;
};

KFC_NAMESPACE_END
