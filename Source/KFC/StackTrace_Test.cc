#include "KFC/StackTrace.h"
#include "KFC/Testing.h"

KFC_NAMESPACE_BEG

void b() {
  void *frames[32];
  const size_t size = getStackTrace(frames, 32, 0);
  const String trace = stringifyStackTrace(frames, size);
  printf("%s\n", trace.c_str());
}

void a() { b(); }

TEST(StackTraceTest, Basic) { a(); }

KFC_NAMESPACE_END
