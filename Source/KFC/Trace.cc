#include "KFC/Trace.h"
#include "KFC/ThreadLocal.h"

#include <stack>

KFC_NAMESPACE_BEG
static thread_local std::stack<Trace> threadLocalTraceStack;
bool GlobalTraceStackEmpty() { return threadLocalTraceStack.empty(); }
void GlobalTraceStackPush(const Trace &trace) { threadLocalTraceStack.push(trace); }
void GlobalTraceStackPop() { threadLocalTraceStack.pop(); }
Trace &GlobalTraceStackTop() { return threadLocalTraceStack.top(); }
KFC_NAMESPACE_END
