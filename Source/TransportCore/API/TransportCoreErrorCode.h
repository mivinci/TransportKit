#ifndef TRANSPORT_CORE_ERROR_CODE_H_
#define TRANSPORT_CORE_ERROR_CODE_H_

enum TransportCoreErrorCode {
  kTransportCoreErrorCodeNone = 0,
  kTransportCoreErrorCodeNetwork,

  kTransportCoreErrorCodeUnknown = 99999999,
};

#endif  // TRANSPORT_CORE_ERROR_CODE_H_
