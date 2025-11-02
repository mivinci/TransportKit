#include "KFC/Timer.h"

KFC_NAMESPACE_BEG

Timer::Timer(const Time &time) : m_time(time) {}

Promise<void> Timer::atTime(Time time) {
  return _::createAdaptedPromise<void, PromiseAdaptor>(*this, time);
}

Promise<void> Timer::afterDelay(const Duration delay) {
  return _::createAdaptedPromise<void, PromiseAdaptor>(*this, m_time + delay);
}

Option<Duration> Timer::advanceTo(const Time &time) {
  m_time = std::max(m_time, time);
  for (;;) {
    auto it = m_events.begin();
    if (it == m_events.end()) break;
    Duration delay = (*it)->m_time - m_time;
    if (delay > 0) {
      return delay;
    }
    (*it)->resolve();
  }
  return None;
}

Timer::PromiseAdaptor::PromiseAdaptor(_::PromiseResolver<void> &resolver, Timer &timer, Time time)
    : m_resolver(resolver), m_timer(timer), m_time(time) {
  m_pos = m_timer.m_events.insert(this);
}

Timer::PromiseAdaptor::~PromiseAdaptor() noexcept(false) {
  if (m_pos != m_timer.m_events.end()) {
    m_timer.m_events.erase(m_pos);
  }
}

void Timer::PromiseAdaptor::resolve() {
  m_resolver.resolve();
  m_timer.m_events.erase(m_pos);
  m_pos = m_timer.m_events.end();
}

bool Timer::PromiseAdaptorComparer::operator()(const PromiseAdaptor *lhs,
                                               const PromiseAdaptor *rhs) const {
  return lhs->m_time < rhs->m_time;
}

KFC_NAMESPACE_END
