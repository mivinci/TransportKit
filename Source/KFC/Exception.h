#pragma once

#include "KFC/Option.h"
#include "KFC/Platform.h"
#include "KFC/String.h"

namespace KFC {
class Exception final : public std::exception {
public:
  explicit Exception();
  explicit Exception(const String &message, const String &scope = "KFC");

  KFC_NODISCARD const char *what() const noexcept override;
};

template <class Func> Option<Exception> runCatchingExceptions(Func &&func) {
  try {
    func();
    return None;
  } catch (Exception &e) {
    return std::move(e);
  } catch (std::exception &e) {
    return Exception(e.what());
  } catch (...) {
    return Exception("Unknown exception");
  }
}
} // namespace KFC
