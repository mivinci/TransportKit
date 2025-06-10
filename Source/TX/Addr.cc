#include "Addr.h"

#include "TX/Result.h"

namespace TX {

Result<SocketAddr, AddrParseError> SocketAddr::Parse(const String &addr) {
  return AddrParseError();
}

Result<IPv6Addr, AddrParseError> IPv6Addr::Parse(const String &addr) {
  return AddrParseError();
}

Result<IPv4Addr, AddrParseError> IPv4Addr::Parse(const String &addr) {
  if (addr.empty()) return AddrParseError();
  const char *ptr = addr.data();
  const char *end = addr.data() + addr.size();
  uint8 octets[4] = {0, 0, 0, 0};
  int i = 0;
  while (ptr < end && i < 4) {
    uint16 octet = 0;
    while (ptr < end && *ptr != '.') {
      if (*ptr < '0' || *ptr > '9') return AddrParseError();
      octet = octet * 10 + (*ptr - '0');
      ++ptr;
    }
    if (octet > 255) return AddrParseError();
    octets[i++] = static_cast<uint8>(octet);
    if (ptr < end) ++ptr;  // skip '.'
  }
  if (i != 4) return AddrParseError();
  return IPv4Addr(octets[0], octets[1], octets[2], octets[3]);
}

}  // namespace TX
