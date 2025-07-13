#include "KFC/Trace.h"
#include "KFC/Test.h"

namespace KFC {
TEST(TraceTest, Trace) {
  KFC_TRACE_START("A");
  usleep(1000);
  KFC_TRACE_SPAN;
  KFC_TRACE_START("A1");
  usleep(1000);
  KFC_TRACE_SPAN;
  KFC_TRACE_END;
  usleep(1000);
  KFC_TRACE_SPAN;
  usleep(1000);
  KFC_TRACE_END;
}
}  // namespace KFC
