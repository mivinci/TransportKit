#pragma once

#include <atomic>

#include "RunLoopThread.h"
#include "TX/Ref.h"
#include "TX/RunLoop.h"

namespace TX {

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

  explicit Ticker(T *ptr, const TickFunc func, const Duration period,
                  const std::string &name)
      : Ticker(ptr, func, period, name, GetGlobalRunLoop()) {}

  explicit Ticker(T *ptr, const TickFunc func, const Duration period,
                  const std::string &name, const Ref<RunLoop> &runLoop)
      : Timer(0, period, kTimerRepeatAlways, name),
        m_ptr(ptr),
        m_func(func),
        m_period(period),
        m_runLoop(runLoop),
        m_active(false) {}

  ~Ticker() override { Stop(); }

  bool Start() {
    bool expected = false;
    if (m_active.compare_exchange_strong(expected, true)) {
      m_runLoop->AddTimer(this);
      return true;
    }
    return false;
  }

  bool Stop() {
    bool expected = true;
    if (m_active.compare_exchange_strong(expected, false)) {
      m_runLoop->RemoveTimer(this);
      return true;
    }
    return false;
  }

  template <class Func>
  void Perform(Func f) {
    m_runLoop->PerformBlock(std::move(f));
  }

  void Perform(OnceFunc0 f) {
    Perform([=] { (m_ptr->*f)(); });
  }
  void Perform(OnceFunc1 f, void *a) {
    Perform([=] { (m_ptr->*f)(a); });
  }
  void Perform(OnceFunc2 f, void *a, void *b) {
    Perform([=] { (m_ptr->*f)(a, b); });
  }
  void Perform(OnceFunc3 f, void *a, void *b, void *c) {
    Perform([=] { (m_ptr->*f)(a, b, c); });
  }
  void Perform(OnceFunc4 f, void *a, void *b, void *c, void *d) {
    Perform([=] { (m_ptr->*f)(a, b, c, d); });
  }
  void Perform(OnceFunc5 f, void *a, void *b, void *c, void *d, void *e) {
    Perform([=] { (m_ptr->*f)(a, b, c, d, e); });
  }

 protected:
  void OnTimeout(const std::string &) override {
    if (m_ptr && m_func) {
      (m_ptr->*m_func)(GetTick());
    }
  }

 private:
  T *m_ptr;
  TickFunc m_func;
  Duration m_period;
  Ref<RunLoop> m_runLoop;
  std::atomic<bool> m_active;
};

}  // namespace TX
