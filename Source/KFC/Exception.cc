#include "KFC/Exception.h"
#include "KFC/Assert.h"
#include "KFC/StackTrace.h"
#include "KFC/ThreadLocal.h"

#include <csignal>
#include <utility>

KFC_NAMESPACE_BEG

static const char *exceptionKindNames[] = {
    [Exception::Kind::Unknown] = "UnknownError",
    [Exception::Kind::UnImplemented] = "UnImplementedError",
    [Exception::Kind::Syscall] = "SyscallError",
    [Exception::Kind::Timeout] = "TimeoutError",
    [Exception::Kind::Logic] = "LogicError",
    [Exception::Kind::Std] = "StdError",
};

static KFC_THREAD_LOCAL char threadLocalExceptionBuffer[2048];

Exception::Exception(const Kind kind, const char *file, const int line, const char *function,
                     std::string message)
    : m_kind(kind), m_line(line), m_file(file), m_function(function),
      m_message(std::move(message)) {
#ifdef TK_SOURCE_DIR
  m_file = m_file.substr(sizeof TK_SOURCE_DIR);
#else
  const uint32_t pos = m_file.find_last_of('/');
  m_file = m_file.substr(pos + 1);
#endif
}

const char *Exception::what() const noexcept {
  snprintf(threadLocalExceptionBuffer, sizeof(threadLocalExceptionBuffer),
           "thrown at %s:%d, in %s\n%s: %s", m_file.c_str(), m_line, m_function.c_str(),
           exceptionKindNames[m_kind], m_message.c_str());
  threadLocalExceptionBuffer[sizeof(threadLocalExceptionBuffer) - 1] = '\0';
  return threadLocalExceptionBuffer;
}

#ifdef _WIN32

#else
KFC_NORETURN void uncaughtExceptionHandler() {
  const String trace = getStackTraceAsString(4);
  try {
    throw; // rethrow
  } catch (const std::exception &e) {
    fprintf(stderr, "Uncaught exception %s\n", e.what());
  } catch (...) {
    fprintf(stderr, "Uncaught exception\n");
  }
  fprintf(stderr, "Stack trace:\n%s", trace.c_str());
  exit(1);
}

KFC_NORETURN void signalHandler(const int signo, siginfo_t *info, void *context) {
  const String trace = getStackTraceAsString(4);
  fprintf(stderr, "Signal %d received\n", signo);
  fprintf(stderr, "Stack trace:\n%s", trace.c_str());
  exit(1);
}

void printStackTraceOnCrash() {
  struct sigaction sa{};
  sa.sa_flags = SA_SIGINFO | SA_RESETHAND | SA_NODEFER;
  sa.sa_sigaction = &signalHandler;
  KFC_CHECK_SYSCALL(sigaction(SIGSEGV, &sa, nullptr));
  KFC_CHECK_SYSCALL(sigaction(SIGABRT, &sa, nullptr));
  KFC_CHECK_SYSCALL(sigaction(SIGILL, &sa, nullptr));
  KFC_CHECK_SYSCALL(sigaction(SIGFPE, &sa, nullptr));
  KFC_CHECK_SYSCALL(sigaction(SIGBUS, &sa, nullptr));
  KFC_CHECK_SYSCALL(sigaction(SIGSYS, &sa, nullptr));
  KFC_CHECK_SYSCALL(sigaction(SIGINT, &sa, nullptr));

  std::set_terminate(&uncaughtExceptionHandler);
}
#endif

void throwFatalException(Exception &&e) {
  fprintf(stderr, "%s\n", e.what());
  const String trace = getStackTraceAsString(2);
  fprintf(stderr, "Stack trace:\n%s", trace.c_str());
  exit(1);
}

KFC_NAMESPACE_END
