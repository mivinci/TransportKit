#pragma once
#include "KFC/Condition.h"
#include "KFC/Mutex.h"

namespace KFC {
class WaitGroup {
public:
  explicit WaitGroup() : m_count(0) {}
  explicit WaitGroup(const int n) : m_count(0) { add(n); }

  void add(const int n) { *m_count.lock() += n; }

  void done() {
    (*m_count.lock())--;
    m_condvar.notifyOne();
  }

  void wait(const Duration &timeout = Duration::FOREVER) {
    auto count_locked = m_count.lock();
    while (*count_locked > 0) {
      m_condvar.wait(count_locked, timeout);
    }
  }

private:
  Mutex<int> m_count;
  Condition m_condvar;
};
} // namespace KFC
