#include "TX/Time.h"
#include "gtest/gtest.h"

namespace TX {
struct DateTimeTestCase {
  int64_t seconds;
  DateTime datetime;
};

static DateTimeTestCase utc_test_cases[] = {
    {0, {1970, January, 1, 0, 0, 0, 0, 0, Thursday, 0}},
    {1221681866, {2008, September, 17, 20, 4, 26, 0, 0, Wednesday, 0}},
    {-1221681866, 1931, April, 16, 3, 55, 34, 0, 0, Thursday, 0},
    {-11644473600, 1601, January, 1, 0, 0, 0, 0, 0, Monday, 0},
    {599529660, 1988, December, 31, 0, 1, 0, 0, 0, Saturday, 0},
    {978220860, 2000, December, 31, 0, 1, 0, 0, 0, Sunday, 0},
};

static DateTimeTestCase local_test_cases[]{

};

TEST(TimeTest, UTC) {
  for (int i = 0; i < std::size(utc_test_cases); i++) {
    const DateTimeTestCase *c = utc_test_cases + i;
    DateTime dt = Time::Unix(c->seconds, 0).UTC().ToDateTime();
    EXPECT_EQ(dt.year, c->datetime.year);
    EXPECT_EQ(dt.month, c->datetime.month);
    EXPECT_EQ(dt.day, c->datetime.day);
    EXPECT_EQ(dt.hour, c->datetime.hour);
    EXPECT_EQ(dt.minute, c->datetime.minute);
    EXPECT_EQ(dt.second, c->datetime.second);
    EXPECT_EQ(dt.nano_seconds, c->datetime.nano_seconds);
    EXPECT_EQ(dt.year, c->datetime.year);
    EXPECT_EQ(dt.week_day, c->datetime.week_day);
    EXPECT_EQ(dt.zone_offset, c->datetime.zone_offset);
  }
}

struct TimeFormatTestCases {
  int64_t sec;
  int64_t nsec;
  const char *format;
  String output;
};

static TimeFormatTestCases format_test_cases[] = {
    {1221681866, 0, RFC3339, "2008-09-17T20:04:26.000Z"},
    {1221681866, 0, RFC3339Nano, "2008-09-17T20:04:26.000000000Z"},
    {1221681866, 123456789, RFC3339, "2008-09-17T20:04:26.123Z"},
    {1221681866, 123456789, RFC3339Nano, "2008-09-17T20:04:26.123456789Z"},
};

TEST(TimeTest, Format) {
  for (int i = 0; i < std::size(format_test_cases); i++) {
    const TimeFormatTestCases *c = format_test_cases + i;
    String output = Time::Unix(c->sec, c->nsec).UTC().Format(c->format);
    EXPECT_EQ(output, c->output);
  }
}

TEST(TimeTest, Arithmetic) {
  // without monotonic
  Time t1(1, 0);
  Time t2 = t1 + 0_s;
  EXPECT_EQ(t2.Unix(), 1);
  t2 = t1 + 1_s;
  EXPECT_EQ(t2.Unix(), 2);

  // with monotonic
  t1 = Time::Now();
  t2 = t1 + 0_s;
  EXPECT_EQ(t1.UnixNano(), t2.UnixNano());
  t2 = t1 + 1_s;
  EXPECT_EQ(t2 - t1, 1_s);
}
}  // namespace TX
