#include "TX/Thread.h"

namespace TX {
#ifdef _WIN32
HANDLE Thread::s_mainThreadId = GetCurrentThread();
#else
pthread_t Thread::s_mainThreadId = pthread_self();
#endif
}  // namespace TX