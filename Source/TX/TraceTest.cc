#include "TX/Trace.h"
#include "gtest/gtest.h"

namespace TX {
TEST(TraceTest, Trace) {
  TX_TRACE_START("A");
  usleep(1000);
  TX_TRACE_SPAN;
  TX_TRACE_START("A1");
  usleep(1000);
  TX_TRACE_SPAN;
  TX_TRACE_END;
  usleep(1000);
  TX_TRACE_SPAN;
  usleep(1000);
  TX_TRACE_END;
}
}  // namespace TX
