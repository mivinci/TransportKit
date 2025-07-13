#pragma once

#include "KFC/Log.h"
#include "KFC/Preclude.h"
#include "KFC/String.h"
#include "KFC/Time.h"

#if defined(ENABLE_TRACE)
#define KFC_TRACE_START(k)                                                                         \
  do {                                                                                             \
    KFC::Trace trace;                                                                              \
    trace.key = k;                                                                                 \
    trace.start = KFC::Time::now();                                                                \
    KFC::GlobalTraceStackPush(trace);                                                              \
    KFC_TRACE("START(%s)", trace.key.c_str());                                                     \
  } while (0)

#define KFC_TRACE_SPAN                                                                             \
  do {                                                                                             \
    if (KFC::GlobalTraceStackEmpty()) break;                                                       \
    KFC::Trace &trace = KFC::GlobalTraceStackTop();                                                \
    trace.elapse = KFC::Time::since(trace.start);                                                  \
    KFC_TRACE("SPAN(%s) elapse: %lf ms", trace.key.c_str(),                                        \
              static_cast<double>(trace.elapse) / 1000000);                                        \
  } while (0)

#define KFC_TRACE_END                                                                              \
  do {                                                                                             \
    KFC::Trace trace = KFC::GlobalTraceStackTop();                                                 \
    KFC_TRACE_SPAN;                                                                                \
    KFC_TRACE("END(%s)", trace.key.c_str());                                                       \
    KFC::GlobalTraceStackPop();                                                                    \
  } while (0)
#else
#define KFC_TRACE_START(k)
#define KFC_TRACE_SPAN
#define KFC_TRACE_END
#endif

KFC_NAMESPACE_BEG
struct Trace {
  String key;
  Time start;
  Duration elapse;
};

bool GlobalTraceStackEmpty();
void GlobalTraceStackPush(const Trace &trace);
void GlobalTraceStackPop();
Trace &GlobalTraceStackTop();
KFC_NAMESPACE_END
