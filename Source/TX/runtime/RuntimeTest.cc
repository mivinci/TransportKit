#include "TX/runtime/Runtime.h"
#include "TX/Test.h"

namespace TX {
TEST(RuntimeTest, BlockOn) {
  Runtime rt = Runtime::SingleThread();
  rt.BlockOn([] { SpawnBlocking([] {}); });
}
}  // namespace TX
