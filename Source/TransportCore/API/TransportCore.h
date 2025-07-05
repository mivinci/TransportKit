#ifndef TRANSPORT_CORE_H_
#define TRANSPORT_CORE_H_
#include <stddef.h>  // NOLINT(*-deprecated-headers)
#include <stdint.h>  // NOLINT(*-deprecated-headers)

#include "TransportCore/API/TransportCoreBase.h"
#include "TransportCore/API/TransportCoreErrorCode.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

enum TransportCoreTaskKind {
  kTransportCoreTaskKindUnSpec = 0,
  kTransportCoreTaskKindPlain,
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
  char *rid;
  char *urls;
  char *save_path;
  void *context;
  void (*schedule)(uint64_t, void *);
  void (*notify)(TransportCoreTaskEvent, void *);
};

TK_API(void) TransportCoreInit();
TK_API(void) TransportCoreDestroy();
TK_API(int32_t) TransportCoreCreateTask(const TransportCoreTaskContext &);
TK_API(void) TransportCoreStartTask(int32_t);
TK_API(void) TransportCoreStopTask(int32_t);
TK_API(void) TransportCorePauseTask(int32_t);
TK_API(void) TransportCoreResumeTask(int32_t);
TK_API(int64_t) TransportCoreReadData(int32_t, int32_t, size_t, size_t, char *);
TK_API(void) TransportCoreGetProxyURL(int32_t, char *, size_t);
TK_API(void)
TransportCoreGetErrorString(TransportCoreErrorCode, char *, size_t);

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // TRANSPORT_CORE_H_
