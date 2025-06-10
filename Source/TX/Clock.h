#pragma once
#include <cstdint>
#ifdef _WIN32
#include <windows.h>
#else
#include <ctime>
#endif

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
  static TimePoint Now(Id id) {
    TimePoint tp{};
#ifdef _WIN32
    switch (id) {
      case Clock::Id::Real:
        FILETIME ft;
        GetSystemTimePreciseAsFileTime(&ft);
        // 将 FileTime 转换为 100 纳秒单位
        ULARGE_INTEGER uli;  //
        uli.LowPart = ft.dwLowDateTime;
        uli.HighPart = ft.dwHighDateTime;
        // 转换为 Unix 时间戳（从 1601-01-01 调整为 1970-01-01）
        uli.QuadPart -= 116444736000000000ULL;
        // 转换为秒和纳秒
        tp.sec = uli.QuadPart / 10'000'000;
        tp.nsec = (uli.QuadPart % 10'000'000) * 100;
        break;
      case Clock::Id::Monotonic:
        static LARGE_INTEGER freq;
        if (!inited) {
          QueryPerformanceFrequency(&freq);
          inited = true;
        }
        LARGE_INTEGER count;
        QueryPerformanceCounter(&count);
        tp.sec = count.QuadPart / freq.QuadPart;
        tp.nsec =
            (count.QuadPart % freq.QuadPart) * 1000000000LL / freq.QuadPart;
        break;
    }
#else
    struct timespec ts{};
    clock_gettime(static_cast<clockid_t>(id), &ts);
    tp.sec = ts.tv_sec;
    tp.nsec = ts.tv_nsec;
#endif
    return tp;
  };

  static TimePoint Real() { return Now(Id::Real); }
  static TimePoint Monotonic() { return Now(Id::Monotonic); }

 private:
#ifdef _WIN32
  static bool inited;
#endif
};

#ifdef _WIN32
inline bool Clock::inited = false;
#endif
}  // namespace TX
