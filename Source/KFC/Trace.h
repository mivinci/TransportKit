#pragma once
#include <stack>
#include <string>

#include "KFC/Assert.h"
#include "KFC/Time.h"

namespace KFC {
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
}  // namespace KFC

#if defined(ENABLE_TRACE)
#define KFC_TRACE_START(k)                     \
  do {                                        \
    KFC::Trace trace;                          \
    trace.key = k;                            \
    trace.start = KFC::Time::Now();            \
    KFC::GlobalTraceStackPush(trace);          \
    KFC_TRACE("START(%s)", trace.key.c_str()); \
  } while (0)
#define KFC_TRACE_SPAN                                      \
  do {                                                     \
    if (KFC::GlobalTraceStackEmpty()) break;                \
    KFC::Trace &trace = KFC::GlobalTraceStackTop();          \
    trace.elapse = KFC::Time::Since(trace.start);           \
    KFC_TRACE("SPAN(%s) elapse: %lf ms", trace.key.c_str(), \
             static_cast<double>(trace.elapse) / 1000000); \
  } while (0)
#define KFC_TRACE_END                             \
  do {                                           \
    KFC::Trace trace = KFC::GlobalTraceStackTop(); \
    KFC_TRACE_SPAN;                               \
    KFC_TRACE("END(%s)", trace.key.c_str());      \
    KFC::GlobalTraceStackPop();                   \
  } while (0)
#else
#define KFC_TRACE_START(k)
#define KFC_TRACE_SPAN
#define KFC_TRACE_END
#endif  // namespace KFC
