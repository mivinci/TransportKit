#include "KFC/Condition.h"

KFC_NAMESPACE_BEG

Condition::Condition() {
#ifdef _WIN32
  InitializeConditionVariable(&cv);
#else
  KFC_CHECK_SYSCALL(pthread_cond_init(&m_cond, nullptr));
#endif
}

void Condition::notifyOne() {
#ifdef _WIN32
  WakeConditionVariable(&cv);
#else
  KFC_CHECK_SYSCALL(pthread_cond_signal(&m_cond));
#endif
}

void Condition::notifyAll() {
#ifdef _WIN32
  WakeAllConditionVariable(&cv);
#else
  KFC_CHECK_SYSCALL(pthread_cond_broadcast(&m_cond));
#endif
}

KFC_NAMESPACE_END
