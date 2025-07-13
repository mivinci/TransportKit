#include "KFC/Addr.h"

#include "KFC/String.h"

#ifdef _WIN32
#include <Windows.h>
#include <in6addr.h>
#include <ip2string.h>
#pragma comment(lib, "ntdll.lib")
#else
#include <arpa/inet.h>
#endif // _WIN32

namespace KFC {

Result<IPv4Addr, AddrParseError> IPv4Addr::parse(const String &str) {
#ifdef _WIN32
  IN_ADDR in;
  PCSTR term = nullptr;
  NTSTATUS rc = RtlIpv4StringToAddress(str.c_str(), TRUE, &term, &in);
  if (rc == 0) return KFC_OK(FromUInt32(ntohl(in.s_addr)));
  if (rc == STATUS_INVALID_PARAMETER) return KFC_ERR(AddrParseError::InvalidIPv4);
  return KFC_ERR(AddrParseError::Internal);
#else
  in_addr in{};
  const int rc = inet_pton(AF_INET, str.c_str(), &in);
  if (rc == 0) return KFC_ERR(AddrParseError::InvalidIPv4);
  if (rc == 1) return KFC_OK(fromInAddr(in));
  return KFC_ERR(AddrParseError::Internal);
#endif // _WIN32
}

Result<IPv6Addr, AddrParseError> IPv6Addr::parse(const String &str) {
#ifdef _WIN32
  IN6_ADDR in;
  PCSTR term = nullptr;
  NTSTATUS rc = RtlIpv6StringToAddress(str.c_str(), &term, &in);
  if (rc == 0) return KFC_OK(FromInAddr(in));
  if (rc == STATUS_INVALID_PARAMETER) return KFC_ERR(AddrParseError::InvalidIPv4);
  return KFC_ERR(AddrParseError::Internal);
#else
  in6_addr in{};
  const int rc = inet_pton(AF_INET6, str.c_str(), &in);
  if (rc == 0) return KFC_ERR(AddrParseError::InvalidIPv6);
  if (rc == 1) return KFC_OK(fromInAddr(in));
  return KFC_ERR(AddrParseError::Internal);
#endif // _WIN32
}

IPv6Addr IPv6Addr::fromInAddr(const IN6_ADDR &in) {
  IPv6Addr addr;
  std::memcpy(addr.m_octets.addr8, in.s6_addr, sizeof(addr.m_octets));
  return addr;
}

IPv4Addr IPv4Addr::fromInAddr(const in_addr &in) { return fromUint32(ntohl(in.s_addr)); }

String IPv4Addr::toString() const {
#ifdef _WIN32
  char buf[INET_ADDRSTRLEN];
  RtlIpv4AddressToStringA((const IN_ADDR *)&m_octets, buf);
  return buf;
#else
  char buf[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &m_octets, buf, sizeof(buf));
  return buf;
#endif // _WIN32
}

String IPv6Addr::toString() const {
#ifdef _WIN32
  char buf[INET6_ADDRSTRLEN];
  RtlIpv6AddressToStringA((const IN6_ADDR *)&m_octets, buf);
  return buf;
#else
  char buf[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET6, &m_octets, buf, sizeof(buf));
  return buf;
#endif // _WIN32
}

Result<IPAddr, AddrParseError> IPAddr::parse(const String &str) {
  const auto it = str.find('.');
  return it == String::npos
             ? IPv6Addr::parse(str).map<IPAddr>([](const IPv6Addr &addr) { return IPAddr(addr); })
             : IPv4Addr::parse(str).map<IPAddr>([](const IPv4Addr &addr) { return IPAddr(addr); });
}

String IPAddr::toString() const {
  if (isV4()) return as<IPv4Addr>().toString();
  return as<IPv6Addr>().toString();
}

Result<SocketAddr, AddrParseError> SocketAddr::parse(const String &str) {
  StringView view(str);
  bool isV6 = false;
  unsigned short port = 0;
  if (view[0] == '[') {
    view = view.slice(1L);
    isV6 = true;
  }
  if (!isV6) {
    const auto it = view.find(':');
    if (it != String::npos) {
      const auto port_str = view.slice(it + 1).toString();
      port = static_cast<unsigned short>(std::stoul(port_str));
    }
    view = view.slice(0U, it);
    return IPv4Addr::parse(view.toString()).map<SocketAddr>([=](const IPv4Addr &addr) {
      return SocketAddr(addr, port);
    });
  } else {
    const auto it = view.find(']');
    if (it != String::npos) {
      if (view[it + 1] != ':') return KFC_ERR(AddrParseError::InvalidIPv6);
      const auto port_str = view.slice(it + 2).toString();
      port = static_cast<unsigned short>(std::stoul(port_str));
    }
    view = view.slice(0U, it);
    return IPv6Addr::parse(view.toString()).map<SocketAddr>([=](const IPv6Addr &addr) {
      return SocketAddr(addr, port);
    });
  }
}

String SocketAddr::toString() const {
  char buf[INET6_ADDRSTRLEN + 9];
  if (isV4()) {
    const SocketAddrV4 addr = as<SocketAddrV4>();
    std::snprintf(buf, sizeof(buf), "%s:%d", addr.ip.toString().c_str(), addr.port);
  } else {
    const SocketAddrV6 addr = as<SocketAddrV6>();
    std::snprintf(buf, sizeof(buf), "[%s]:%d", addr.ip.toString().c_str(), addr.port);
  }
  return buf;
}

} // namespace KFC
