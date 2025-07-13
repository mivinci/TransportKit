#pragma once
#include "KFC/Option.h"
#include "KFC/Preclude.h"

KFC_NAMESPACE_BEG
template <class Func> Option<Exception> runCatchingExceptions(Func &&func) {
  try {
    func();
    return None;
  } catch (Exception &e) {
    return std::move(e);
  } catch (std::exception &e) {
    return KFC_EXCEPTION(KFC::Exception::Kind::Std, e.what());
  } catch (...) {
    return KFC_EXCEPTION(KFC::Exception::Kind::Unknown, "Unknown exception");
  }
}
KFC_NAMESPACE_END
