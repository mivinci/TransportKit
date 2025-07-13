#include "KFC/Time.h"

KFC_NAMESPACE_BEG

Duration Duration::FOREVER = Duration(INT64_MAX);
bool Time::isZoneSet = false;

int64_t Duration::toNanoSeconds() const { return m_elapse; }
int64_t Duration::toMicroSeconds() const { return m_elapse / 1000; }
int64_t Duration::toMilliSeconds() const { return m_elapse / 1000000; }

double Duration::toSeconds() const {
  const int64_t sec = m_elapse / 1000000000;
  const int64_t nsec = m_elapse % 1000000000;
  return static_cast<double>(sec) + static_cast<double>(nsec) / 1e9;
}

double Duration::toMinutes() const {
  const int64_t min = m_elapse / (60 * 1000000000LL);
  const int64_t nsec = m_elapse % (60 * 1000000000LL);
  return static_cast<double>(min) + static_cast<double>(nsec) / (60 * 1e9);
}

double Duration::toHours() const {
  const int64_t hour = m_elapse / (60 * 60 * 1000000000LL);
  const int64_t nsec = m_elapse % (60 * 60 * 1000000000LL);
  return static_cast<double>(hour) + static_cast<double>(nsec) / (60 * 60 * 1e9);
}

Clock::TimePoint Duration::toTimePoint() const {
  Clock::TimePoint tp{};
  const int64_t sec = m_elapse / 1000000000LL;
  const int64_t nsec = m_elapse % 1000000000LL;
  tp.sec = sec;
  tp.nsec = nsec;
  return tp;
}

Time::Time(const int64_t ns) : m_wall(0), m_ext(0), m_utc(false) {
  const int64_t u_sec = ns / 1000000000;
  const int64_t u_nsec = ns % 1000000000;
  m_wall = static_cast<uint64_t>(u_nsec);
  m_ext = u_sec;
}

Time::Time(const int64_t sec, const int64_t nsec) : m_utc(false) {
  m_wall = static_cast<uint64_t>(nsec);
  m_ext = sec;
}

Time Time::fromUnix(const int64_t sec, const int64_t nsec) { return Time(sec, nsec); }
Time Time::after(const Duration elapse) { return now() + elapse; }
Time Time::before(const Duration elapse) { return now() - elapse; }
Time Time::now() {
  Time t;
  readNow(t);
  return t;
}

Duration Time::since(const Time t) { return now() - t; }
Duration Time::until(const Time t) { return t - now(); }

uint64_t Time::toUnix() const { return sec(); }
uint64_t Time::toUnixMilli() const { return sec() * 1000 + nsec() / 1000000; }
uint64_t Time::toUnixMicro() const { return sec() / 1000000 + nsec() / 1000; }
uint64_t Time::toUnixNano() const { return sec() * 1000000000 + nsec(); }

Clock::TimePoint Time::toTimePoint() const {
  Clock::TimePoint tp{};
  tp.sec = sec();
  tp.nsec = nsec();
  return tp;
}

Time Time::toUTC() const {
  Time t = *this;
  t.m_utc = true;
  return t;
}

Time Time::operator+(const Duration d) const {
  if (d == 0) return *this;
  Time t = *this;
  int64_t d_sec = d.m_elapse / 1000000000;
  int32_t t_nsec = nsec() + static_cast<int32_t>(d.m_elapse % 1000000000);
  if (t_nsec >= 1000000000) {
    d_sec++;
    t_nsec -= 1000000000;
  } else if (t_nsec < 0) {
    d_sec--;
    t_nsec += 1000000000;
  }
  t.m_wall = (t.m_wall & (~kNSecMask)) | static_cast<uint64_t>(t_nsec);
  t.addSec(d_sec);
  if ((t.m_wall & kHasMono) != 0) {
    const int64_t te = t.m_ext + d.m_elapse;
    if ((d.m_elapse < 0 && te > t.m_ext) || (d.m_elapse > 0 && te < t.m_ext)) {
      // Monotonic clock reading now out of range; degrade to wall-only
      t.stripMono();
    } else {
      t.m_ext = te;
    }
  }
  return t;
}

Time Time::operator-(const Duration d) const { return operator+(-d.m_elapse); }

Duration Time::operator-(const Time &other) const {
  if ((m_wall & other.m_wall & kHasMono) != 0) return subMono(m_ext, other.m_ext);
  const Duration d = (sec() - other.sec()) * 1000000000 + (nsec() - other.nsec());
  // Check for overflow or underflow.
  const Time u = other;
  if (u + d == *this) return d;
  if (*this < u) return INT64_MIN;
  return INT64_MAX;
}

bool Time::operator==(const Time &other) const {
  return ((m_wall & other.m_wall & kHasMono) != 0)
             ? m_ext == other.m_ext
             : (sec() == other.sec() && nsec() == other.nsec());
}

bool Time::operator>(const Time &other) const {
  if ((m_wall & other.m_wall & kHasMono) != 0) return m_ext > other.m_ext;
  const int64_t t_sec = sec();
  const int64_t u_sec = other.nsec();
  return t_sec > u_sec || (t_sec == u_sec && nsec() > other.nsec());
}

bool Time::operator<(const Time &other) const {
  if ((m_wall & other.m_wall & kHasMono) != 0) return m_ext < other.m_ext;
  const int64_t t_sec = sec();
  const int64_t u_sec = other.nsec();
  return t_sec < u_sec || (t_sec == u_sec && nsec() < other.nsec());
}

DateTime Time::toDateTime() const {
  struct tm tm {};
  TM(&tm);

  struct DateTime dt {};
  dt.year = tm.tm_year + 1900;
  dt.month = static_cast<Month>(tm.tm_mon + 1);
  dt.day = tm.tm_mday;
  dt.hour = tm.tm_hour;
  dt.minute = tm.tm_min;
  dt.second = tm.tm_sec;
  dt.nano_seconds = nsec();
  dt.year_day = tm.tm_yday;
  dt.week_day = static_cast<WeekDay>(tm.tm_wday);
#ifdef _WIN32
  static char tzname[32];
  long off = 0;
  TIME_ZONE_INFORMATION tzi;
  const DWORD rc = GetTimeZoneInformation(&tzi);
  if (rc != TIME_ZONE_ID_INVALID) off -= tzi.Bias;
  if (rc == TIME_ZONE_ID_DAYLIGHT) off -= tzi.DaylightBias;
  ::memcpy(tzname, tzi.StandardName, sizeof(tzname));
  dt.zone_offset = off * 60;
  dt.zone_name = tzname;
#else
  if (!isZoneSet) {
    tzset();
    isZoneSet = true;
  }
  dt.zone_offset = tm.tm_gmtoff;
  dt.zone_name = tzname[0]; // UTC time zone name
#endif
  return dt;
}

std::string Time::toString(const std::string &layout) const {
  char buf[64];
  struct tm tm {};
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

int64_t Time::sec() const {
  return (m_wall & kHasMono) != 0 ? static_cast<int64_t>((m_wall << 1) >> (kNSecShift + 1)) : m_ext;
}
int32_t Time::nsec() const { return static_cast<int32_t>(m_wall & kNSecMask); }
int64_t Time::mono() const { return (m_wall & kHasMono) != 0 ? m_ext : 0; }

void Time::addSec(const int64_t d) {
  if ((m_wall & kHasMono) != 0) {
    const auto sec = static_cast<int64_t>((m_wall << 1) >> (kNSecShift + 1));
    const int64_t t_sec = sec + d;
    if (0 <= t_sec && t_sec <= (1LL << 33) - 1) {
      m_wall = m_wall & kNSecMask | static_cast<uint64_t>(t_sec) << kNSecShift | kHasMono;
      return;
    }
    // Wall second now out of range for packed field. Move to ext.
    stripMono();
  }
  // Check if the sum of t.ext and d overflows and handle it properly.
  const int64_t sum = m_ext + d;
  if ((sum > m_ext) == (d > 0))
    m_ext = sum;
  else if (d > 0)
    m_ext = INT64_MAX;
  else
    m_ext = -INT64_MAX;
}

int64_t Time::subMono(const int64_t t, const int64_t u) const {
  const int64_t d = t - u;
  if (d < 0 && t > u) return INT64_MAX;
  if (d > 0 && t < u) return INT64_MIN;
  return d;
}

void Time::stripMono() {
  if ((m_wall & kHasMono) != 0) {
    m_ext = sec();
    m_wall &= kNSecMask;
  }
}

void Time::TM(struct tm *tm) const {
  const time_t t_sec = sec();
  if (m_utc) {
#ifdef _WIN32
    gmtime_s(tm, &t_sec);
#else
    gmtime_r(&t_sec, tm);
#endif
  } else {
#ifdef _WIN32
    localtime_s(tm, &t_sec);
#else
    localtime_r(&t_sec, tm);
#endif
  }
}

size_t Time::formatRFC3339(const struct tm *tm, char *buf, size_t, const bool nano) const {
  char *p = buf;
  size_t n = 0;

  n = strftime(p, 64, RFC3339, tm);
  if (n <= 8) return 0; // e.g. 0800_xxx
  p += (n - 8);

  if (nano) {
    p += snprintf(p, 11, ".%09d", nsec());
  } else {
    p += snprintf(p, 5, ".%03d", nsec() / 1000000);
  }

  long off = 0;
#ifdef _WIN32
  TIME_ZONE_INFORMATION tzi;
  const DWORD rc = GetTimeZoneInformation(&tzi);
  if (rc != TIME_ZONE_ID_INVALID) off -= tzi.Bias;
  if (rc == TIME_ZONE_ID_DAYLIGHT) off -= tzi.DaylightBias;
#else
  off = tm->tm_gmtoff / 60; // zone offset in minutes
#endif
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

void Time::readNow(Time &t) {
  const Clock::TimePoint tp_wall = Clock::real();
  const Clock::TimePoint tp_mono = Clock::monotonic();
  const int64_t mono = tp_mono.sec * 1000000000 + tp_mono.nsec;
  if (static_cast<uint64_t>(tp_wall.sec) >> 33) {
    // Seconds field overflowed the 33 bits available when storing a monotonic
    // time. This will be true after May 21. 2242
    t.m_wall = static_cast<uint64_t>(tp_wall.nsec);
    t.m_ext = tp_wall.sec;
    return;
  }
  t.m_wall = kHasMono | (static_cast<uint64_t>(tp_wall.sec) << kNSecShift) |
             static_cast<uint64_t>(tp_wall.nsec);
  t.m_ext = mono;
}

KFC_NAMESPACE_END
