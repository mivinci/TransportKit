#pragma once

#include "KFC/Format.h"
#include "KFC/Preclude.h"
#include <exception>

#define KFC_THROW(k, ...) throw(KFC_EXCEPTION(k, __VA_ARGS__))
#define KFC_EXCEPTION(k, ...)                                                                      \
  KFC_NAMESPACE::Exception(k, __FILE__, __LINE__, __FUNCTION__, KFC_FORMAT(__VA_ARGS__))

KFC_NAMESPACE_BEG

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

  explicit Exception(Kind kind, const char *file, int line, const char *function,
                     std::string message);

  KFC_NODISCARD const char *what() const noexcept override;

private:
  Kind m_kind;
  int m_line;
  std::string m_file;
  std::string m_function;
  std::string m_message;
};

void printStackTraceOnCrash();
KFC_NOINLINE KFC_NORETURN void throwRecoverableException(Exception &&e);
KFC_NOINLINE void throwFatalException(Exception &&e);

KFC_NAMESPACE_END
