#pragma once

#include "KFC/Option.h"
#include "KFC/Platform.h"
#include "KFC/String.h"

#define KFC_EXCEPTION(...) ::KFC::Exception(__FILE__, __LINE__, KFC_FORMAT(__VA_ARGS__))

namespace KFC {
class Exception final : public std::exception {
public:
  explicit Exception();
  explicit Exception(const char *file, int line, const String &message);

  KFC_NODISCARD const char *what() const noexcept override;
};

template <class Func> Option<Exception> runCatchingExceptions(Func &&func) {
  try {
    func();
    return None;
  } catch (Exception &e) {
    return std::move(e);
  } catch (std::exception &e) {
    return KFC_EXCEPTION(e.what());
  } catch (...) {
    return KFC_EXCEPTION("Unknown exception");
  }
}

KFC_NOINLINE KFC_NORETURN void throwFatalException(Exception &&e, int ignore = 0);
} // namespace KFC
