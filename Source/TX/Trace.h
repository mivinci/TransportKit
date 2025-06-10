#pragma once
#include <stack>
#include <string>

#include "TX/Assert.h"
#include "TX/Time.h"

namespace TX {
struct Trace {
  std::string key;
  Time start;
  Duration elapse;
};
static thread_local std::stack<Trace> trace_stack;
#if defined(ENABLE_TRACE)
#define TX_TRACE_START(k)                     \
  do {                                        \
    Trace trace;                              \
    trace.key = k;                            \
    trace.start = Time::Now();                \
    trace_stack.push(trace);                  \
    TX_TRACE("START(%s)", trace.key.c_str()); \
  } while (0)
#define TX_TRACE_SPAN                                      \
  do {                                                     \
    TX_ASSERT(!trace_stack.empty());                       \
    Trace &trace = trace_stack.top();                      \
    trace.elapse = Time::Since(trace.start);               \
    TX_TRACE("SPAN(%s) elapse: %lf ms", trace.key.c_str(), \
             static_cast<double>(trace.elapse) / 1000000); \
  } while (0)
#define TX_TRACE_END                        \
  do {                                      \
    Trace trace = trace_stack.top();        \
    TX_TRACE_SPAN;                          \
    TX_TRACE("END(%s)", trace.key.c_str()); \
    trace_stack.pop();                      \
  } while (0)
#else
#define TX_TRACE_START(k)
#define TX_TRACE_SPAN
#define TX_TRACE_END
#endif
}  // namespace TX
