#ifndef TRANSPORT_CORE_BASE_H_
#define TRANSPORT_CORE_BASE_H_

#undef TK_EXPORT
#if defined(TC_STATIC)
#define TK_EXPORT
#elif defined(_WIN32) || defined(__CYGWIN__)
#if defined(BUILD_TK)
#define TK_EXPORT __declspec(dllexport)
#else
#define TK_EXPORT __declspec(dllimport)
#endif  // defined(BUILD_TK)
#elif defined(__GNUC__) || defined(__clang__)
#define TK_EXPORT __attribute__((visibility("default")))
#endif  // defined(TC_STATIC)

#if defined(_WIN32)
#include <windows.h>
#define TK_STDCALL __stdcall
#else
#define TK_STDCALL
#endif  // defined(_WIN32)

#define TK_API(T) TK_EXPORT T TK_STDCALL
#endif  // TRANSPORT_CORE_BASE_H_
