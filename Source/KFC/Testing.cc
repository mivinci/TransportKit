#include "KFC/Testing.h"
#include "KFC/Exception.h"

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  KFC::PrintStackTraceOnCrash();
  return RUN_ALL_TESTS();
}
