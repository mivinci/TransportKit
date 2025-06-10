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
static thread_local std::stack<Trace> gTraceStack;
inline bool GlobalTraceStackEmpty() { return gTraceStack.empty(); }
inline void GlobalTraceStackPush(const Trace &t) { gTraceStack.push(t); }
inline void GlobalTraceStackPop() { gTraceStack.pop(); }
inline Trace &GlobalTraceStackTop() { return gTraceStack.top(); }
}  // namespace TX

#if defined(ENABLE_TRACE)
#define TX_TRACE_START(k)                     \
  do {                                        \
    TX::Trace trace;                          \
    trace.key = k;                            \
    trace.start = TX::Time::Now();            \
    TX::GlobalTraceStackPush(trace);          \
    TX_TRACE("START(%s)", trace.key.c_str()); \
  } while (0)
#define TX_TRACE_SPAN                                      \
  do {                                                     \
    if (TX::GlobalTraceStackEmpty()) break;                \
    TX::Trace &trace = TX::GlobalTraceStackTop();          \
    trace.elapse = TX::Time::Since(trace.start);           \
    TX_TRACE("SPAN(%s) elapse: %lf ms", trace.key.c_str(), \
             static_cast<double>(trace.elapse) / 1000000); \
  } while (0)
#define TX_TRACE_END                             \
  do {                                           \
    TX::Trace trace = TX::GlobalTraceStackTop(); \
    TX_TRACE_SPAN;                               \
    TX_TRACE("END(%s)", trace.key.c_str());      \
    TX::GlobalTraceStackPop();                   \
  } while (0)
#else
#define TX_TRACE_START(k)
#define TX_TRACE_SPAN
#define TX_TRACE_END
#endif  // namespace TX
