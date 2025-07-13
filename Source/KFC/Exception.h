#pragma once

#include "KFC/Format.h"
#include "KFC/Preclude.h"
#include <exception>

#define KFC_THROW(k, ...) throw KFC_EXCEPTION(k, __VA_ARGS__)
#define KFC_EXCEPTION(k, ...)                                                                      \
  KFC_NAMESPACE::Exception(k, __FILE__, __LINE__, __FUNCTION__, KFC_FORMAT(__VA_ARGS__))

KFC_NAMESPACE_BEG

enum {
  kUnknownError = 0,
  kSyscallError,
  kInvalidArgument,
  kTypeError,
  kUnImplementedError,
  kLogicError,
  kStdError,
};

class Exception final : public std::exception {
public:
  explicit Exception(int kind, const char *file, int line, const char *function,
                     std::string message);

  KFC_NODISCARD const char *what() const noexcept override;

private:
  int m_kind;
  std::string m_file;
  int m_line;
  std::string m_function;
  std::string m_message;
};

void PrintStackTraceOnCrash();

KFC_NAMESPACE_END
