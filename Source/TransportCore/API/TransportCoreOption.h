#ifndef TRANSPORT_CORE_OPTION_H_
#define TRANSPORT_CORE_OPTION_H_

#include "TransportCore/API/TransportCoreBase.h"

enum TransportCoreOption {
  kTransportCoreOptionLocalServerPort,
};

TK_API(void) TransportCoreSetGlobalOption(enum TransportCoreOption, ...);

#endif  // TRANSPORT_CORE_OPTION_H_
