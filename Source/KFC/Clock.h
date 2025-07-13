#pragma once
#include <cstdint>

namespace KFC {
using Tick = uint64_t;

class Clock {
public:
  enum struct Id {
    Real = 0,
    Monotonic = 6,
  };

  struct TimePoint {
    int64_t sec;
    int64_t nsec;
  };

  static TimePoint now(Id id);
  static TimePoint real() { return now(Id::Real); }
  static TimePoint monotonic() { return now(Id::Monotonic); }

private:
#ifdef _WIN32
  static bool m_inited;
#endif
};
} // namespace KFC
