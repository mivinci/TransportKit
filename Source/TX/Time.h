#pragma once
#include "TX/Clock.h"
#include "TX/Platform.h"
#include "TX/String.h"

namespace TX {
constexpr uint64_t kNSecShift = 30;
constexpr uint64_t kNSecMask = (1ULL << kNSecShift) - 1;
constexpr uint64_t kHasMono = 1ULL << 63;
constexpr uint64_t kMonoMask = kHasMono - 1;
constexpr uint64_t kSecondsPerMinute = 60;
constexpr uint64_t kSecondsPerHour = 60 * 60;
constexpr uint64_t kSecondsPerDay = 60 * 60 * 24;

class Duration final {
 public:
  constexpr Duration() : inner_(0) {}
  // ReSharper disable once CppNonExplicitConvertingConstructor
  constexpr Duration(const int64_t n) /* NOLINT(*-explicit-constructor) */
      : inner_(n) {}
  static constexpr Duration NanoSecond(const int64_t n) { return n; }
  static constexpr Duration MicroSecond(const int64_t n) { return n * 1000; }
  static constexpr Duration MilliSecond(const int64_t n) { return n * 1000000; }
  static constexpr Duration Second(const int64_t n) { return n * 1000000000; }
  static constexpr Duration Minute(const int64_t n) {
    return n * 1000000000 * 60;
  }
  static constexpr Duration Hour(const int64_t n) {
    return n * 1000000000 * 60 * 60;
  }
  static Duration FOREVER;

  TX_NODISCARD int64_t NanoSeconds() const { return inner_; }
  TX_NODISCARD int64_t MicroSeconds() const { return inner_ / 1000; }
  TX_NODISCARD int64_t MilliSeconds() const { return inner_ / 1000000; }

  TX_NODISCARD double Seconds() const {
    const int64_t sec = inner_ / 1000000000;
    const int64_t nsec = inner_ % 1000000000;
    return static_cast<double>(sec) + static_cast<double>(nsec) / 1e9;
  }
  TX_NODISCARD double Minutes() const {
    const int64_t min = inner_ / (60 * 1000000000LL);
    const int64_t nsec = inner_ % (60 * 1000000000LL);
    return static_cast<double>(min) + static_cast<double>(nsec) / (60 * 1e9);
  }
  TX_NODISCARD double Hours() const {
    const int64_t hour = inner_ / (60 * 60 * 1000000000LL);
    const int64_t nsec = inner_ % (60 * 60 * 1000000000LL);
    return static_cast<double>(hour) +
           static_cast<double>(nsec) / (60 * 60 * 1e9);
  }

  explicit operator double() const { return static_cast<double>(inner_); }
  explicit operator float() const { return static_cast<float>(inner_); }

  Duration operator-() const { return -inner_; }

  Duration operator+(const Duration &other) const {
    return inner_ + other.inner_;
  }
  Duration operator-(const Duration &other) const {
    return inner_ - other.inner_;
  }
  Duration operator*(const Duration &other) const {
    return inner_ * other.inner_;
  }
  Duration operator/(const Duration &other) const {
    return inner_ / other.inner_;
  }
  Duration operator%(const Duration &other) const {
    return inner_ % other.inner_;
  }
  Duration &operator+=(const Duration &other) {
    inner_ += other.inner_;
    return *this;
  }
  Duration &operator-=(const Duration &other) {
    inner_ -= other.inner_;
    return *this;
  }

  bool operator==(const Duration &other) const {
    return inner_ == other.inner_;
  }
  bool operator<(const Duration &other) const { return inner_ < other.inner_; }
  bool operator>(const Duration &other) const { return inner_ > other.inner_; }
  bool operator<=(const Duration &other) const {
    return inner_ <= other.inner_;
  }
  bool operator>=(const Duration &other) const {
    return inner_ >= other.inner_;
  }

  TX_NODISCARD Clock::TimePoint ToTimePoint() const {
    Clock::TimePoint tp{};
    const int64_t sec = inner_ / 1000000000LL;
    const int64_t nsec = inner_ % 1000000000LL;
    tp.sec = sec;
    tp.nsec = nsec;
    return tp;
  }

 private:
  friend class Time;
  int64_t inner_;
};

inline Duration Duration::FOREVER = Duration(INT64_MAX);

inline bool operator!=(const Duration &lhs, const Duration &rhs) {
  return !(lhs == rhs);
}

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
  long zone_offset;  // seconds east of UTC
  char *zone_name;
};

// Ref: https://github.com/golang/go/blob/master/src/time/time.go
class Time final {
 public:
  explicit Time() : wall_(0), ext_(0), utc_(false) {}
  explicit Time(const int64_t ns) : wall_(0), ext_(0), utc_(false) {
    const int64_t u_sec = ns / 1000000000;
    const int64_t u_nsec = ns % 1000000000;
    wall_ = static_cast<uint64_t>(u_nsec);
    ext_ = u_sec;
  }

  explicit Time(const int64_t sec, const int64_t nsec) : utc_(false) {
    wall_ = static_cast<uint64_t>(nsec);
    ext_ = sec;
  }

  Time(const Time &) = default;
  Time(Time &&) = default;
  Time &operator=(const Time &) = default;
  Time &operator=(Time &&) = default;

  TX_NODISCARD Time UTC() const {
    Time t = *this;
    t.utc_ = true;
    return t;
  }

  TX_NODISCARD uint64_t Unix() const { return sec(); }
  TX_NODISCARD uint64_t UnixMilli() const {
    return sec() * 1000 + nsec() / 1000000;
  }
  TX_NODISCARD uint64_t UnixMicro() const {
    return sec() / 1000000 + nsec() / 1000;
  }
  TX_NODISCARD uint64_t UnixNano() const { return sec() * 1000000000 + nsec(); }

  TX_NODISCARD Clock::TimePoint ToTimePoint() const {
    return {.sec = sec(), .nsec = nsec()};
  }
  static Time Unix(const int64_t sec, const int64_t nsec) {
    return Time(sec, nsec);
  }

  Time operator+(const Duration d) const {
    if (d == 0) return *this;
    Time t = *this;
    int64_t d_sec = d.inner_ / 1000000000;
    int32_t t_nsec = nsec() + static_cast<int32_t>(d.inner_ % 1000000000);
    if (t_nsec >= 1000000000) {
      d_sec++;
      t_nsec -= 1000000000;
    } else if (t_nsec < 0) {
      d_sec--;
      t_nsec += 1000000000;
    }
    t.wall_ = (t.wall_ & (~kNSecMask)) | static_cast<uint64_t>(t_nsec);
    t.addSec(d_sec);
    if ((t.wall_ & kHasMono) != 0) {
      const int64_t te = t.ext_ + d.inner_;
      if ((d.inner_ < 0 && te > t.ext_) || (d.inner_ > 0 && te < t.ext_)) {
        // Monotonic clock reading now out of range; degrade to wall-only
        t.stripMono();
      } else {
        t.ext_ = te;
      }
    }
    return t;
  }

  Time operator-(const Duration d) const { return operator+(-d.inner_); }

  Duration operator-(const Time &other) const {
    if ((wall_ & other.wall_ & kHasMono) != 0) return subMono(ext_, other.ext_);
    const Duration d =
        (sec() - other.sec()) * 1000000000 + (nsec() - other.nsec());
    // Check for overflow or underflow.
    const Time u = other;
    if (u + d == *this) return d;
    if (*this < u) return INT64_MIN;
    return INT64_MAX;
  }

  bool operator==(const Time &other) const {
    return ((wall_ & other.wall_ & kHasMono) != 0)
               ? ext_ == other.ext_
               : (sec() == other.sec() && nsec() == other.nsec());
  }

  bool operator>(const Time &other) const {
    if ((wall_ & other.wall_ & kHasMono) != 0) return ext_ > other.ext_;
    const int64_t t_sec = sec();
    const int64_t u_sec = other.nsec();
    return t_sec > u_sec || (t_sec == u_sec && nsec() > other.nsec());
  }

  bool operator<(const Time &other) const {
    if ((wall_ & other.wall_ & kHasMono) != 0) return ext_ < other.ext_;
    const int64_t t_sec = sec();
    const int64_t u_sec = other.nsec();
    return t_sec < u_sec || (t_sec == u_sec && nsec() < other.nsec());
  }

  TX_NODISCARD DateTime ToDateTime() const {
    if (!zone_set) {
      tzset();
      zone_set = true;
    }

    struct tm tm{};
    TM(&tm);

    struct DateTime dt{};
    dt.year = tm.tm_year + 1900;
    dt.month = static_cast<Month>(tm.tm_mon + 1);
    dt.day = tm.tm_mday;
    dt.hour = tm.tm_hour;
    dt.minute = tm.tm_min;
    dt.second = tm.tm_sec;
    dt.nano_seconds = nsec();
    dt.year_day = tm.tm_yday;
    dt.week_day = static_cast<WeekDay>(tm.tm_wday);
    dt.zone_offset = tm.tm_gmtoff;
    dt.zone_name = tzname[0];  // UTC time zone name
    return dt;
  }

  TX_NODISCARD String Format(const StringView &layout = RFC3339) const {
    char buf[64];
    struct tm tm{};
    TM(&tm);

    if (layout == RFC3339) {
      formatRFC3339(&tm, buf, 64, false);
    } else if (layout == RFC3339Nano) {
      formatRFC3339(&tm, buf, 64, true);
    } else {
      strftime(buf, 64, layout.data(), &tm);
    }
    return buf;
  }

  static Time After(const Duration elapse) { return Now() + elapse; }
  static Time Before(const Duration elapse) { return Now() - elapse; }
  static Duration Since(const Time t) { return Now() - t; }
  static Duration Until(const Time t) { return t - Now(); }
  static Time Now() {
    Time t;
    now(t);
    return t;
  }

 private:
  TX_NODISCARD int64_t sec() const {
    return ((wall_ & kHasMono) != 0)
               ? static_cast<int64_t>((wall_ << 1) >> (kNSecShift + 1))
               : ext_;
  }

  TX_NODISCARD int32_t nsec() const {
    return static_cast<int32_t>(wall_ & kNSecMask);
  }

  TX_NODISCARD int64_t mono() const {
    return ((wall_ & kHasMono) != 0) ? ext_ : 0;
  }

  void addSec(const int64_t d) {
    if ((wall_ & kHasMono) != 0) {
      const auto sec = static_cast<int64_t>((wall_ << 1) >> (kNSecShift + 1));
      const int64_t t_sec = sec + d;
      if (0 <= t_sec && t_sec <= (1LL << 33) - 1) {
        wall_ = (wall_ & kNSecMask) | (static_cast<uint64_t>(t_sec) << kNSecShift) |
                kHasMono;
        return;
      }
      // Wall second now out of range for packed field. Move to ext.
      stripMono();
    }
    // Check if the sum of t.ext and d overflows and handle it properly.
    const int64_t sum = ext_ + d;
    if ((sum > ext_) == (d > 0))
      ext_ = sum;
    else if (d > 0)
      ext_ = INT64_MAX;
    else
      ext_ = -INT64_MAX;
  }

  TX_NODISCARD int64_t subMono(const int64_t t, const int64_t u)
      const { /* NOLINT(readability-convert-member-functions-to-static) */
    const int64_t d = t - u;
    if (d < 0 && t > u) return INT64_MAX;
    if (d > 0 && t < u) return INT64_MIN;
    return d;
  }

  void stripMono() {
    if ((wall_ & kHasMono) != 0) {
      ext_ = sec();
      wall_ &= kNSecMask;
    }
  }

  void TM(struct tm *tm) const {
    const time_t t_sec = sec();
    if (utc_) {
      gmtime_r(&t_sec, tm);
    } else {
      localtime_r(&t_sec, tm);
    }
  }

  size_t formatRFC3339(const struct tm *tm, char *buf, size_t,
                       const bool nano) const {
    char *p = buf;
    size_t n = 0;

    n = strftime(p, 64, RFC3339, tm);
    if (n <= 8) return 0;  // e.g. 0800_xxx
    p += (n - 8);

    if (nano) {
      p += snprintf(p, 11, ".%09d", nsec());
    } else {
      p += snprintf(p, 5, ".%03d", nsec() / 1000000);
    }

    long off = tm->tm_gmtoff / 60;  // zone offset in minutes
    if (off == 0) {
      *p++ = 'Z';
      *p++ = '\0';
      return p - buf;
    }
    if (off < 0) {
      *p++ = '-';
      off = -off;
    } else {
      *p++ = '+';
    }
    p += snprintf(p, 6, "%02ld:%02ld", off / 60, off % 60);
    *p++ = '\0';
    return p - buf;
  }

  static void now(Time &t) {
    Clock::TimePoint wall_tp = Clock::Real();
    Clock::TimePoint mono_tp = Clock::Monotonic();
    const int64_t mono = mono_tp.sec * 1000000000 + mono_tp.nsec;
    if (static_cast<uint64_t>(wall_tp.sec) >> 33) {
      // Seconds field overflowed the 33 bits available when storing a monotonic
      // time. This will be true after May 21. 2242
      t.wall_ = static_cast<uint64_t>(wall_tp.nsec);
      t.ext_ = wall_tp.sec;
      return;
    }
    t.wall_ = kHasMono | (static_cast<uint64_t>(wall_tp.sec) << kNSecShift) |
              static_cast<uint64_t>(wall_tp.nsec);
    t.ext_ = mono;
  }

 private:
  uint64_t wall_;
  int64_t ext_;
  bool utc_;

  static bool zone_set;
};

inline bool Time::zone_set = false;
}  // namespace TX

constexpr TX::Duration operator""_ns(const unsigned long long n) {
  return TX::Duration::NanoSecond(static_cast<int64_t>(n));
}
constexpr TX::Duration operator""_us(const unsigned long long n) {
  return TX::Duration::MicroSecond(static_cast<int64_t>(n));
}
constexpr TX::Duration operator""_ms(const unsigned long long n) {
  return TX::Duration::MilliSecond(static_cast<int64_t>(n));
}
constexpr TX::Duration operator""_s(const unsigned long long n) {
  return TX::Duration::Second(static_cast<int64_t>(n));
}
constexpr TX::Duration operator""_m(const unsigned long long n) {
  return TX::Duration::Minute(static_cast<int64_t>(n));
}
constexpr TX::Duration operator""_h(const unsigned long long n) {
  return TX::Duration::Hour(static_cast<int64_t>(n));
}
