#ifndef TRANSPORT_CORE_H_
#define TRANSPORT_CORE_H_
#include <stddef.h>  // NOLINT(*-deprecated-headers)
#include <stdint.h>  // NOLINT(*-deprecated-headers)

#include "TransportCoreBase.h"
#include "TransportCoreErrorCode.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

enum TransportCoreTaskKind {
  kTransportCoreTaskKindPlain = 1,
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

TC_API(void) TransportCoreInit();
TC_API(void) TransportCoreDestroy();
TC_API(int32_t) TransportCoreCreateTask(const TransportCoreTaskContext &);
TC_API(void) TransportCoreStartTask(int32_t);
TC_API(void) TransportCoreStopTask(int32_t);
TC_API(void) TransportCorePauseTask(int32_t);
TC_API(void) TransportCoreResumeTask(int32_t);
TC_API(int64_t) TransportCoreReadData(int32_t, size_t, size_t, char *, size_t);
TC_API(void) TransportCoreGetProxyURL(int32_t, char *, size_t);
TC_API(void) TransportCoreGetErrorString(int, char *, size_t);

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // TRANSPORT_CORE_H_
