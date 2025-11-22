#pragma once

#include "KFC/Preclude.h"
#include "KFC/Time.h"

KFC_NAMESPACE_BEG

template <class T> class ScheduleHandle {
public:
  typedef void (T::*Schedule)(Tick);
  typedef void (T::*Func0)();
  typedef void (T::*Func1)(void *);
  typedef void (T::*Func2)(void *, void *);
  typedef void (T::*Func3)(void *, void *, void *);
  typedef void (T::*Func4)(void *, void *, void *, void *);
  typedef void (T::*Func5)(void *, void *, void *, void *, void *);

  explicit ScheduleHandle(T *ptr, const Schedule schedule, const Duration period)
      : m_ptr(ptr), m_schedule(schedule), m_period(period) {}

  bool start() {}
  bool stop() {}

  void queueWork(Func0 f) {}
  void queueWork(Func1 f) {}
  void queueWork(Func2 f) {}
  void queueWork(Func3 f) {}
  void queueWork(Func4 f) {}
  void queueWork(Func5 f) {}

private:
  T *m_ptr;
  Schedule m_schedule;
  Duration m_period;
};

KFC_NAMESPACE_END
