#include "KFC/Bits.h"
#include "KFC/Preclude.h"
#include "KFC/Testing.h"

KFC_NAMESPACE_BEG

TEST(BitsTest, countTrailingZeros) {
  EXPECT_EQ(countTrailingZeros(0), 32);
  EXPECT_EQ(countTrailingZeros(1), 0);
  EXPECT_EQ(countTrailingZeros(2), 1);
  EXPECT_EQ(countTrailingZeros(3), 0);
  EXPECT_EQ(countTrailingZeros(4), 2);
  EXPECT_EQ(countTrailingZeros(UINT32_MAX), 0);
}

KFC_NAMESPACE_END