#pragma once
#include "TX/Condvar.h"
#include "TX/Mutex.h"

namespace TX {
class WaitGroup {
 public:
  explicit WaitGroup() : count_(0) {}
  explicit WaitGroup(const int n) : count_(0) { Add(n); }
  void Add(const int n) { (*count_.Lock()) += n; }
  void Done() {
    (*count_.Lock())--;
    cond_.NotifyOne();
  }
  void Wait(const Duration& timeout = Duration::FOREVER) {
    auto count_locked = count_.Lock();
    while (*count_locked > 0) {
      cond_.Wait(count_locked, timeout);
    }
  }

 private:
  Mutex<int> count_;
  Condvar cond_;
};
}  // namespace TX
