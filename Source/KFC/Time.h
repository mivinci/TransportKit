#pragma once
#include <cstdio>
#include <ctime>
#include <string>

#include "KFC/Clock.h"
#include "KFC/Preclude.h"

#ifdef _WIN32
#include <Windows.h>
#endif

KFC_NAMESPACE_BEG
constexpr uint64_t kNSecShift = 30;
constexpr uint64_t kNSecMask = (1ULL << kNSecShift) - 1;
constexpr uint64_t kHasMono = 1ULL << 63;
constexpr uint64_t kMonoMask = kHasMono - 1;
constexpr uint64_t kSecondsPerMinute = 60;
constexpr uint64_t kSecondsPerHour = 60 * 60;
constexpr uint64_t kSecondsPerDay = 60 * 60 * 24;

class Duration final {
public:
  constexpr Duration() : m_elapse(0) {}
  constexpr Duration(const int64_t n) : m_elapse(n) {}

  static constexpr Duration fromNanoSecond(const int64_t n) { return n; }
  static constexpr Duration fromMicroSecond(const int64_t n) { return n * 1000; }
  static constexpr Duration fromMilliSecond(const int64_t n) { return n * 1000000; }
  static constexpr Duration fromSecond(const int64_t n) { return n * 1000000000; }
  static constexpr Duration fromMinute(const int64_t n) { return n * 1000000000 * 60; }
  static constexpr Duration fromHour(const int64_t n) { return n * 1000000000 * 60 * 60; }
  static Duration FOREVER;

  KFC_NODISCARD int64_t toNanoSeconds() const;
  KFC_NODISCARD int64_t toMicroSeconds() const;
  KFC_NODISCARD int64_t toMilliSeconds() const;
  KFC_NODISCARD double toSeconds() const;
  KFC_NODISCARD double toMinutes() const;
  KFC_NODISCARD double toHours() const;
  KFC_NODISCARD Clock::TimePoint toTimePoint() const;

  explicit operator double() const { return static_cast<double>(m_elapse); }
  explicit operator float() const { return static_cast<float>(m_elapse); }

  Duration operator-() const { return -m_elapse; }

  Duration operator+(const Duration &other) const { return m_elapse + other.m_elapse; }
  Duration operator-(const Duration &other) const { return m_elapse - other.m_elapse; }
  Duration operator*(const Duration &other) const { return m_elapse * other.m_elapse; }
  Duration operator/(const Duration &other) const { return m_elapse / other.m_elapse; }
  Duration operator%(const Duration &other) const { return m_elapse % other.m_elapse; }
  Duration &operator+=(const Duration &other) {
    m_elapse += other.m_elapse;
    return *this;
  }
  Duration &operator-=(const Duration &other) {
    m_elapse -= other.m_elapse;
    return *this;
  }

  bool operator==(const Duration &other) const { return m_elapse == other.m_elapse; }
  bool operator<(const Duration &other) const { return m_elapse < other.m_elapse; }
  bool operator>(const Duration &other) const { return m_elapse > other.m_elapse; }
  bool operator<=(const Duration &other) const { return m_elapse <= other.m_elapse; }
  bool operator>=(const Duration &other) const { return m_elapse >= other.m_elapse; }

private:
  friend class Time;
  int64_t m_elapse;
};

inline bool operator!=(const Duration &lhs, const Duration &rhs) { return !(lhs == rhs); }

enum Month {
  January = 1,
  February,
  March,
  April,
  May,
  June,
  July,
  August,
  September,
  October,
  November,
  December,
};

enum WeekDay {
  Sunday = 0,
  Monday,
  Tuesday,
  Wednesday,
  Thursday,
  Friday,
  Saturday,
};

constexpr auto RFC822 = "%d %b %y %H:%M %Z";
constexpr auto RFC3339 = "%Y-%m-%dT%H:%M:%S%z__s";
constexpr auto RFC3339Nano = "%Y-%m-%dT%H:%M:%S%z_ns";

struct DateTime {
  int year;
  Month month;
  int day;
  int hour;
  int minute;
  int second;
  int nano_seconds;
  int year_day;
  WeekDay week_day;
  long zone_offset; // seconds east of UTC
  char *zone_name;
};

// The design of `Time` follows the Go's standard `time` library.
// See https://github.com/golang/go/blob/master/src/time/time.go
// because it can hold both the wall-time and the monotonic time.
class Time final {
public:
  constexpr explicit Time() : m_wall(0), m_ext(0), m_utc(false) {}
  explicit Time(int64_t ns);
  explicit Time(int64_t sec, int64_t nsec);
  Time(const Time &) = default;
  Time(Time &&) = default;
  Time &operator=(const Time &) = default;
  Time &operator=(Time &&) = default;

  static Time fromUnix(int64_t sec, int64_t nsec);
  static Time after(Duration elapse);
  static Time before(Duration elapse);
  static Time now();
  static Duration since(Time t);
  static Duration until(Time t);

  KFC_NODISCARD uint64_t toUnix() const;
  KFC_NODISCARD uint64_t toUnixMilli() const;
  KFC_NODISCARD uint64_t toUnixMicro() const;
  KFC_NODISCARD uint64_t toUnixNano() const;
  KFC_NODISCARD Clock::TimePoint toTimePoint() const;
  KFC_NODISCARD Time toUTC() const;
  KFC_NODISCARD DateTime toDateTime() const;
  KFC_NODISCARD std::string toString(const std::string &layout = RFC3339) const;

  Time operator+(Duration d) const;
  Time operator-(Duration d) const;
  Duration operator-(const Time &other) const;
  bool operator==(const Time &other) const;
  bool operator>(const Time &other) const;
  bool operator<(const Time &other) const;

private:
  KFC_NODISCARD int64_t sec() const;
  KFC_NODISCARD int32_t nsec() const;
  KFC_NODISCARD int64_t mono() const;

  void addSec(int64_t d);
  int64_t subMono(int64_t t, int64_t u) const;
  void stripMono();
  void TM(struct tm *tm) const;
  size_t formatRFC3339(const struct tm *tm, char *buf, size_t, bool nano) const;
  static void readNow(Time &t);

  uint64_t m_wall;
  int64_t m_ext;
  bool m_utc;

  static bool isZoneSet;
};

KFC_NAMESPACE_END

constexpr KFC_NAMESPACE::Duration operator""_ns(const unsigned long long n) {
  return KFC_NAMESPACE::Duration::fromNanoSecond(static_cast<int64_t>(n));
}
constexpr KFC_NAMESPACE::Duration operator""_us(const unsigned long long n) {
  return KFC_NAMESPACE::Duration::fromMicroSecond(static_cast<int64_t>(n));
}
constexpr KFC_NAMESPACE::Duration operator""_ms(const unsigned long long n) {
  return KFC_NAMESPACE::Duration::fromMilliSecond(static_cast<int64_t>(n));
}
constexpr KFC_NAMESPACE::Duration operator""_s(const unsigned long long n) {
  return KFC_NAMESPACE::Duration::fromSecond(static_cast<int64_t>(n));
}
constexpr KFC_NAMESPACE::Duration operator""_m(const unsigned long long n) {
  return KFC_NAMESPACE::Duration::fromMinute(static_cast<int64_t>(n));
}
constexpr KFC_NAMESPACE::Duration operator""_h(const unsigned long long n) {
  return KFC_NAMESPACE::Duration::fromHour(static_cast<int64_t>(n));
}
