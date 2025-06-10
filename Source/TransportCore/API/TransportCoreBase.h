#ifndef TRANSPORT_CORE_BASE_H_
#define TRANSPORT_CORE_BASE_H_
#undef TC_EXPORT
#if defined(TC_STATIC)
#define TC_EXPORT
#elif defined(_WIN32) || defined(__CYGWIN__)
#if defined(BUILD_TC)
#define TC_EXPORT __declspec(dllexport)
#else
#define TC_EXPORT __declspec(dllimport)
#endif  // defined(BUILD_TC)
#elif defined(__GNUC__) || defined(__clang__)
#define TC_EXPORT __attribute__((visibility("default")))
#endif  // defined(TC_STATIC)

#if defined(_WIN32)
#include <windows.h>
#define TK_STDCALL __stdcall
#else
#define TC_STDCALL
#endif  // defined(_WIN32)

#define TC_API(T) TC_EXPORT T TC_STDCALL
#endif  // TRANSPORT_CORE_BASE_H_
