#pragma once

#include <functional>

#include "KFC/Ref.h"
#include "KFC/RunLoop.h"

namespace KFC {

class Timer final : RefCounted<Timer>, RunLoop::Timer {
 public:
  explicit Timer(std::function<void()> f, const Duration delay)
      : RunLoop::Timer(delay), m_func(std::move(f)) {
    KFC_CURRENT_RUNLOOP.addTimer(this);
  }

  void cancel() {
    if (!isAlive()) return;
    KFC_CURRENT_RUNLOOP.removeTimer(this);
  }

  void onTimeout(const RunLoop &) override { m_func(); }
  ~Timer() override { cancel(); }

 private:
  std::function<void()> m_func;
};

#define KFC_DISPATCH_TIMER(f, delay) (KFC::Timer(f, delay))
#define KFC_CANCEL_TIMER(timer) (timer->cancel())

}  // namespace KFC
