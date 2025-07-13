#ifndef TRANSPORT_CORE_ERROR_CODE_H_
#define TRANSPORT_CORE_ERROR_CODE_H_

#include <stddef.h>

#include "TransportCore/API/TransportCoreBase.h"

#define TK_RESULT int
#define TK_OK 0
#define TK_ERR (-1)

enum TransportCoreErrorCode {
  kTransportCoreErrorCodeNone = 0,
  kTransportCoreErrorCodeUnInitialized,
  kTransportCoreErrorCodeNetwork,

  kTransportCoreErrorCodeUnknown = 99999999,
};

typedef enum TransportCoreErrorCode TransportCoreErrorCode;

TK_API(void)
TransportCoreGetErrorString(TransportCoreErrorCode, char *, size_t);

#endif  // TRANSPORT_CORE_ERROR_CODE_H_
