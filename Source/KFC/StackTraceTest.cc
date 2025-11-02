#include "KFC/StackTrace.h"
#include "KFC/Testing.h"

KFC_NAMESPACE_BEG

void bar() {
  const String trace = getStackTraceAsString();
  printf("%s\n", trace.c_str());
}

void foo() { bar(); }

TEST(StackTraceTest, GetStackTrace) { foo(); }

KFC_NAMESPACE_END
