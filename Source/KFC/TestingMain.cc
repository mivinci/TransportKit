#include "KFC/Exception.h"
#include "KFC/Testing.h"

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  KFC::printStackTraceOnCrash();
  return RUN_ALL_TESTS();
}
