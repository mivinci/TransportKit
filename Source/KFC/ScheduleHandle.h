#pragma once

#include "KFC/Preclude.h"
#include "KFC/Time.h"

KFC_NAMESPACE_BEG

template <class T> class ScheduleHandle {
public:
  typedef void (T::*Schedule)(Tick);
  explicit ScheduleHandle(T *ptr, const Schedule schedule, const Duration period)
      : m_ptr(ptr), m_schedule(schedule), m_period(period) {}

  bool start() {}
  bool stop() {}

private:
  T *m_ptr;
  Schedule m_schedule;
  Duration m_period;
};

KFC_NAMESPACE_END
