#pragma once

#include "KFC/Async.h"
#include "KFC/Option.h"
#include "KFC/Preclude.h"
#include "KFC/Time.h"

#include <set>

KFC_NAMESPACE_BEG

class Timer final {
public:
  explicit Timer(Time time);
  Option<Duration> advanceTo(Time time);
  Promise<void> atTime(Time time);
  Promise<void> afterDelay(Duration delay);

private:
  class PromiseAdaptor;
  struct PromiseAdaptorComparer {
    bool operator()(const PromiseAdaptor *lhs, const PromiseAdaptor *rhs) const;
  };

  std::multiset<PromiseAdaptor *, PromiseAdaptorComparer> m_events;
  Time m_time;

  friend PromiseAdaptor;
};

class Timer::PromiseAdaptor final {
public:
  explicit PromiseAdaptor(_::PromiseResolver<void> &resolver, Timer &timer, Time time);
  ~PromiseAdaptor() noexcept(false);
  void resolve();

private:
  _::PromiseResolver<void> &m_resolver;
  std::multiset<PromiseAdaptor *, PromiseAdaptorComparer>::iterator m_pos;
  Timer &m_timer;
  Time m_time;

  friend Timer;
  friend PromiseAdaptorComparer;
};

KFC_NAMESPACE_END
