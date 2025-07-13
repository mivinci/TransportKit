#include "KFC/String.h"
#include "KFC/Test.h"

namespace KFC {
TEST(StringTest, Slice) {
  StringView s0;
  EXPECT_TRUE(s0.empty());

  s0 = "12345678";
  StringView s1 = s0;
  EXPECT_TRUE(s1 == "12345678");

  s1 = s0.slice(0, 0);
  EXPECT_TRUE(s1.empty());

  s1 = s0.slice(0, 1);
  EXPECT_TRUE(s1 == "1");
  s1 = s0.slice(0, 2);
  EXPECT_TRUE(s1 == "12");
  s1 = s0.slice(0, 8);
  EXPECT_TRUE(s1 == "12345678");
  s1 = s0.slice(0, 9);
  EXPECT_TRUE(s1 == "12345678");

  s1 = s0.slice(0, -1);
  EXPECT_TRUE(s1 == "1234567");
  s1 = s0.slice(0, -2);
  EXPECT_TRUE(s1 == "123456");
  s1 = s0.slice(0, -8);
  EXPECT_TRUE(s1.empty());
  s1 = s0.slice(0, -9);
  EXPECT_TRUE(s1.empty());

  s1 = s0.slice(0);
  EXPECT_TRUE(s1 == "12345678");
  s1 = s0.slice(1);
  EXPECT_TRUE(s1 == "2345678");
  s1 = s0.slice(2);
  EXPECT_TRUE(s1 == "345678");
  s1 = s0.slice(8);
  EXPECT_TRUE(s1.empty());
  s1 = s0.slice(9);
  EXPECT_TRUE(s1.empty());

  s1 = s0.slice(-1);
  EXPECT_TRUE(s1 == "8");
  s1 = s0.slice(-2);
  EXPECT_TRUE(s1 == "78");
  s1 = s0.slice(-8);
  EXPECT_TRUE(s1 == "12345678");
  s1 = s0.slice(-9);
  EXPECT_TRUE(s1 == "12345678");

  s1 = s0.slice(-1, 0);
  EXPECT_TRUE(s1.empty());
  s1 = s0.slice(-2, 0);
  EXPECT_TRUE(s1.empty());
  s1 = s0.slice(-2, -1);
  EXPECT_TRUE(s1 == "7");
  s1 = s0.slice(-5, 7);
  EXPECT_TRUE(s1 == "4567");
}

TEST(StringTest, Find) {
  const StringView s0("12345678");

  EXPECT_EQ(s0.find('1'), 0);
  EXPECT_EQ(s0.find('8'), 7);
  EXPECT_EQ(s0.find('9'), std::string::npos);

  EXPECT_EQ(s0.find("12"), 0);
  EXPECT_EQ(s0.find("78"), 6);
  EXPECT_EQ(s0.find("abc"), std::string::npos);
}

TEST(StringTest, Iterator) {
  const StringView s0("12345678");
  EXPECT_EQ(*s0.begin(), '1');
  EXPECT_EQ(*s0.end(), '\0');

  std::string str;
  for (const char c : s0) {
    str.push_back(c);
  }
  EXPECT_STREQ(str.c_str(), "12345678");
}
}  // namespace KFC