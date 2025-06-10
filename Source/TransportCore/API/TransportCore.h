#ifndef TRANSPORT_CORE_H_
#define TRANSPORT_CORE_H_
#include <stdint.h>  // NOLINT(*-deprecated-headers)
#include <stddef.h>  // NOLINT(*-deprecated-headers)

#undef TK_EXPORT
#if defined(TK_STATIC)
#define TK_EXPORT
#elif defined(_WIN32) || defined(__CYGWIN__)
#if defined(BUILD_TK)
#define TK_EXPORT __declspec(dllexport)
#else
#define TK_EXPORT __declspec(dllimport)
#endif  // defined(BUILD_TK)
#elif defined(__GNUC__) || defined(__clang__)
#define TK_EXPORT __attribute__((visibility("default")))
#endif  // defined(TK_STATIC)

#if defined(_WIN32)
#include <windows.h>
#define TK_STDCALL __stdcall
#else
#define TK_STDCALL
#endif  // defined(_WIN32)

#define TK_API(T) TK_EXPORT T TK_STDCALL

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

enum TransportCoreErrorCode {
  kTransportCoreErrorCodeNone = 0,
  kTransportCoreErrorCodeUnknown,
  kTransportCoreErrorCodeNetwork,
};

enum TransportCoreTaskKind {
  kTransportCoreTaskKindPlain = 0,
};

enum TransportCoreTaskEventKind {
  kTransportCoreTaskEventKindStart = 0,
  kTransportCoreTaskEventKindStop,
  kTransportCoreTaskEventKindPause,
  kTransportCoreTaskEventKindResume,
  kTransportCoreTaskEventKindProgress,
  kTransportCoreTaskEventKindError,
};

struct TransportCoreTaskEvent {
  TransportCoreTaskEventKind kind;
  TransportCoreErrorCode err_code;
  int32_t task_id;
  int32_t speed;
};

struct TransportCoreTaskContext {
  TransportCoreTaskKind kind;
  char *urls;
  char *save_path;
  void *context;
  void (*onEvent)(TransportCoreTaskEvent, void *);
};

TK_API(void) TransportCoreInit();
TK_API(void) TransportCoreDestroy();
TK_API(int32_t) TransportCoreCreateTask(TransportCoreTaskContext);
TK_API(void) TransportCoreStartTask(int32_t);
TK_API(void) TransportCoreStopTask(int32_t);
TK_API(void) TransportCorePauseTask(int32_t);
TK_API(void) TransportCoreResumeTask(int32_t);
TK_API(int64_t) TransportCoreReadData(int32_t, size_t, size_t, char *, size_t);
TK_API(void) TransportCoreGetProxyURL(int32_t, char *, size_t);
TK_API(void) TransportCoreGetErrorString(TransportCoreErrorCode, char *, size_t);

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // TRANSPORT_CORE_H_
