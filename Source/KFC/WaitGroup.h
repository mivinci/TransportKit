#pragma once
#include "KFC/Condvar.h"
#include "KFC/Mutex.h"

namespace KFC {
class WaitGroup {
 public:
  explicit WaitGroup() : count_(0) {}
  explicit WaitGroup(const int n) : count_(0) { add(n); }

  void add(const int n) { *count_.lock() += n; }

  void done() {
    (*count_.lock())--;
    cond_.notifyOne();
  }

  void wait(const Duration& timeout = Duration::FOREVER) {
    auto count_locked = count_.lock();
    while (*count_locked > 0) {
      cond_.wait(count_locked, timeout);
    }
  }

 private:
  Mutex<int> count_;
  Condvar cond_;
};
}  // namespace KFC
