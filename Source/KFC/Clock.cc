#include "KFC/Clock.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <ctime>
#endif

namespace KFC {
#ifdef _WIN32
bool Clock::m_inited = false;
#endif

Clock::TimePoint Clock::now(Id id) {
  TimePoint tp{};
#ifdef _WIN32
  switch (id) {
    case Id::Real:
      FILETIME ft;
      GetSystemTimePreciseAsFileTime(&ft);
      // 将 FileTime 转换为 100 纳秒单位
      ULARGE_INTEGER uli;  //
      uli.LowPart = ft.dwLowDateTime;
      uli.HighPart = ft.dwHighDateTime;
      // 转换为 Unix 时间戳（从 1601-01-01 调整为 1970-01-01）
      uli.QuadPart -= 116444736000000000ULL;
      // 转换为秒和纳秒
      tp.sec = static_cast<int64_t>(uli.QuadPart / 10'000'000);
      tp.nsec = static_cast<int64_t>((uli.QuadPart % 10'000'000)) * 100;
      break;
    case Id::Monotonic:
      static LARGE_INTEGER freq;
      if (!m_inited) {
        QueryPerformanceFrequency(&freq);
        m_inited = true;
      }
      LARGE_INTEGER count;
      QueryPerformanceCounter(&count);
      tp.sec = count.QuadPart / freq.QuadPart;
      tp.nsec = (count.QuadPart % freq.QuadPart) * 1000000000LL / freq.QuadPart;
      break;
  }
#else
  struct timespec ts{};
  clock_gettime(static_cast<clockid_t>(id), &ts);
  tp.sec = ts.tv_sec;
  tp.nsec = ts.tv_nsec;
#endif
  return tp;
}

}  // namespace KFC
