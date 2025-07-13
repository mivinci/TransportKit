#pragma once
#include <cstdint>

namespace TX {
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

  static TimePoint Now(Id id);
  static TimePoint Real() { return Now(Id::Real); }
  static TimePoint Monotonic() { return Now(Id::Monotonic); }

 private:
#ifdef _WIN32
  static bool m_inited;
#endif
};
}  // namespace TX
