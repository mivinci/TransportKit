#include "KFC/Result.h"
#include "KFC/Testing.h"

namespace KFC {
enum class MockError : int { E };

TEST(ResultTest, Simple) {
  Result<int, MockError> r1 = KFC_OK(42);
  Result<String, MockError> r2 = KFC_OK(String("42"));
  EXPECT_EQ(r1.unwrap(), 42);
  EXPECT_EQ(r2.unwrap(), "42");

  r1 = KFC_ERR(MockError::E);
  r2 = KFC_ERR(MockError::E);
  EXPECT_EQ(r1.unwrapErr(), MockError::E);
  EXPECT_EQ(r2.unwrapErr(), MockError::E);

  EXPECT_EQ(r1.unwrapOr(114514), 114514);
  EXPECT_EQ(r2.unwrapOr(String("114514")), "114514");
}

} // namespace KFC
