#include "KFC/OneOf.h"
#include "KFC/String.h"
#include "KFC/Testing.h"

namespace KFC {
TEST(UnionTest, Simple) {
  OneOf<int, String> u;
  EXPECT_EQ(u.emplace<int>(42), 42);
  EXPECT_EQ(u.index(), 0);
}
} // namespace KFC
