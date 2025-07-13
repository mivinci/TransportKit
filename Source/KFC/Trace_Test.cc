#include "KFC/Testing.h"
#include "KFC/Trace.h"

KFC_NAMESPACE_BEG

TEST(TraceTest, Simple) {
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

KFC_NAMESPACE_END
