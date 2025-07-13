#include "KFC/String.h"
#include "KFC/Test.h"
#include "KFC/Union.h"

namespace KFC {
TEST(UnionTest, Simple) {
  Union<int, String> u;
  EXPECT_EQ(u.emplace<int>(42), 42);
  EXPECT_EQ(u.index(), 0);
}
}  // namespace KFC
