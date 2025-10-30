#pragma once

#include "KFC/CopyMove.h"
#include "KFC/Format.h"
#include "KFC/Preclude.h"
#include <exception>

// Throws a fatal exception.
#define KFC_THROW_FATAL(k, ...) KFC_NAMESPACE::throwFatalException(KFC_EXCEPTION(k, __VA_ARGS__))

// Throws a recoverable exception.
#define KFC_THROW_RECOVERABLE(k, ...)                                                              \
  KFC_NAMESPACE::throwRecoverableException(KFC_EXCEPTION(k, __VA_ARGS__))

// Constructs an exception.
#define KFC_EXCEPTION(k, ...)                                                                      \
  KFC_NAMESPACE::Exception(k, __FILE__, __LINE__, __FUNCTION__, KFC_FORMAT(__VA_ARGS__))

KFC_NAMESPACE_BEG

// Extends `std::exception` to support thread-level exception handling. KFC introduces two kinds of
// exceptions: fatal exceptions and recoverable exceptions. A fatal exception will be handled by the
// thread local fatal exception callback, and then terminate the process by calling `abort()`, while
// a recoverable exception will be handled by the thread local recoverable exception callback and
// then continue to execute. If no thread local exception callback is set, the global default one
// will be used. The global default fatal exception callback just simply throws the exception by
// C++'s keyword `throw`, while the global default recoverable exception callback just does nothing.
//
// Example:
//
// A fatal exception can be thrown by:
//
//   KFC_THROW_FATAL(kLogic, "I am a fatal exception");
//
// A recoverable exception can be thrown by:
//
//   KFC_THROW_RECOVERABLE(kLogic, "I am a recoverable exception");
//
// Developers should always know if it is a fatal or a recoverable exception they are throwing. For
// example, a system call fails, in which case there's no way to recover, we must throw a fatal
// exception to let the process crash.
class Exception final : public std::exception {
public:
  enum Kind {
    Unknown,
    UnImplemented,
    Syscall,
    Timeout,
    Logic,
    Std,
  };

  class Callback {
  public:
    KFC_DISALLOW_COPY_AND_MOVE(Callback)
    Callback();
    virtual ~Callback() noexcept(false);
    virtual void onFatalException(Exception &&e);
    virtual void onRecoverableException(Exception &&e);

  private:
    Callback *prev_;
  };

  explicit Exception(Kind kind, const char *file, int line, const char *function, String message);

  KFC_NODISCARD const char *what() const noexcept override;

  // Returns the message of the exception. When the exception is thrown via:
  //
  //   KFC_THROW_FATAL(kLogic, "abc %d", 123);
  //
  // The message will be "abc 123".
  KFC_NODISCARD String getMessage() const;

private:
  Kind m_kind;
  int m_line;
  String m_file;
  String m_function;
  String m_message;
};

void printStackTraceOnCrash();
KFC_NOINLINE void throwRecoverableException(Exception &&e);
KFC_NOINLINE KFC_NORETURN void throwFatalException(Exception &&e);

KFC_NAMESPACE_END
