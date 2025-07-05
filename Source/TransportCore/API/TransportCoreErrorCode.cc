#include "TransportCoreErrorCode.h"

#include <algorithm>
#include <cstring>
#include <unordered_map>

// TODO: use a simple struct + binary search
static const std::unordered_map<int, const char *> gErrorCodeStringMap = {
    {kTransportCoreErrorCodeNone, "none"},
    {kTransportCoreErrorCodeNetwork, "network error"},
    {kTransportCoreErrorCodeUnknown, "unknown error"},
};

void TransportCoreGetErrorString(const TransportCoreErrorCode err_code,
                                 char *buf, size_t size) {
  const auto it = gErrorCodeStringMap.find(err_code);
  if (it == gErrorCodeStringMap.end()) return;
  size = std::min(size, strlen(it->second));
  memcpy(buf, it->second, size);
  buf[size - 1] = '\0';
}
