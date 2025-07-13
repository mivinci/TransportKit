#pragma once

#include <atomic>

#include "KFC/RunLoop.h"

namespace KFC {

template <class T>
class Ticker final : public RunLoop::Timer {
 public:
  typedef void (T::*TickFunc)(Tick);
  typedef void (T::*OnceFunc0)();
  typedef void (T::*OnceFunc1)(void *);
  typedef void (T::*OnceFunc2)(void *, void *);
  typedef void (T::*OnceFunc3)(void *, void *, void *);
  typedef void (T::*OnceFunc4)(void *, void *, void *, void *);
  typedef void (T::*OnceFunc5)(void *, void *, void *, void *, void *);

  explicit Ticker(T *ptr, const TickFunc func, const Duration period, const String &name)
      : Ticker(ptr, func, period, name, KFC_CURRENT_RUNLOOP) {}

  explicit Ticker(T *ptr, const TickFunc func, const Duration period, const String &name,
                  RunLoop &runLoop)
      : Timer(0, period, kRunLoopTimerRepeatAlways, name),
        m_ptr(ptr),
        m_func(func),
        m_period(period),
        m_runLoop(runLoop),
        m_active(false) {}

  ~Ticker() override { stop(); }

  bool start() {
    bool expected = false;
    if (m_active.compare_exchange_strong(expected, true)) {
      m_runLoop.addTimer(this);
      return true;
    }
    return false;
  }

  bool stop() {
    bool expected = true;
    if (m_active.compare_exchange_strong(expected, false)) {
      m_runLoop.removeTimer(this);
      return true;
    }
    return false;
  }

  template <class Func>
  void perform(Func f) {
    m_runLoop.performBlock(std::move(f));
  }

  void perform(OnceFunc0 f) {
    perform([=] { (m_ptr->*f)(); });
  }
  void perform(OnceFunc1 f, void *a) {
    perform([=] { (m_ptr->*f)(a); });
  }
  void perform(OnceFunc2 f, void *a, void *b) {
    perform([=] { (m_ptr->*f)(a, b); });
  }
  void perform(OnceFunc3 f, void *a, void *b, void *c) {
    perform([=] { (m_ptr->*f)(a, b, c); });
  }
  void perform(OnceFunc4 f, void *a, void *b, void *c, void *d) {
    perform([=] { (m_ptr->*f)(a, b, c, d); });
  }
  void perform(OnceFunc5 f, void *a, void *b, void *c, void *d, void *e) {
    perform([=] { (m_ptr->*f)(a, b, c, d, e); });
  }

 protected:
  void onTimeout(const RunLoop &) override {
    if (m_ptr && m_func) {
      (m_ptr->*m_func)(ticks());
    }
  }

 private:
  T *m_ptr;
  TickFunc m_func;
  Duration m_period;
  RunLoop &m_runLoop;
  std::atomic<bool> m_active;
};

}  // namespace KFC
