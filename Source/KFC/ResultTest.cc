#include "KFC/Result.h"
#include "KFC/Test.h"

namespace KFC {
enum class MockError : int { E };

TEST(ResultTest, Simple) {
  Result<int, MockError> r1 = KFC_OK(42);
  Result<std::string, MockError> r2 = KFC_OK(std::string("42"));
  EXPECT_EQ(r1.unwrap(), 42);
  EXPECT_EQ(r2.unwrap(), "42");

  r1 = KFC_ERR(MockError::E);
  r2 = KFC_ERR(MockError::E);
  EXPECT_EQ(r1.unwrapErr(), MockError::E);
  EXPECT_EQ(r2.unwrapErr(), MockError::E);

  EXPECT_EQ(r1.unwrapOr(114514), 114514);
  EXPECT_EQ(r2.unwrapOr(std::string("114514")), "114514");
  EXPECT_EQ(r1.unwrapOr([] { return 114514; }), 114514);
  EXPECT_EQ(r2.unwrapOr([] { return std::string("114514"); }), "114514");
}

}  // namespace KFC
