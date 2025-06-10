#include "TX/runtime/Runtime.h"
#include "gtest/gtest.h"

namespace TX {
TEST(RuntimeTest, BlockOn) {
  Runtime rt;
  rt.BlockOn([] {

  });
}
}  // namespace TX