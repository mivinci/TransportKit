#include "KFC/String.h"
#include "KFC/Testing.h"

namespace KFC {
TEST(StringTest, slice) {
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

TEST(StringTest, find) {
  const StringView s0("12345678");

  EXPECT_EQ(s0.find('1'), 0);
  EXPECT_EQ(s0.find('8'), 7);
  EXPECT_EQ(s0.find('9'), String::npos);

  EXPECT_EQ(s0.find("12"), 0);
  EXPECT_EQ(s0.find("78"), 6);
  EXPECT_EQ(s0.find("abc"), String::npos);
}

TEST(StringTest, rfind) {
  const StringView s0("12345678");

  EXPECT_EQ(s0.rfind('1'), 0);
  EXPECT_EQ(s0.rfind('8'), 7);
  EXPECT_EQ(s0.rfind('9'), String::npos);

  EXPECT_EQ(s0.rfind("12"), 0);
  EXPECT_EQ(s0.rfind("78"), 6);
  EXPECT_EQ(s0.rfind("abc"), String::npos);
}

TEST(StringTest, iterator) {
  const StringView s0("12345678");
  EXPECT_EQ(*s0.begin(), '1');
  EXPECT_EQ(*s0.end(), '\0');

  String str;
  for (const char c : s0) {
    str.push_back(c);
  }
  EXPECT_STREQ(str.c_str(), "12345678");
}

TEST(StringTest, joinStringArray) {
  std::vector<String> arr1 = {"1", "2", "3"};
  EXPECT_EQ(joinStringArray(arr1, ","), "1,2,3");

  String arr2[] = {"1", "2", "3"};
  EXPECT_EQ(joinStringArray(arr2, ","), "1,2,3");
}
} // namespace KFC