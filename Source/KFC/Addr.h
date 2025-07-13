#pragma once

#include "KFC/String.h"
#ifdef _WIN32
#include <Windows.h>
#include <in6addr.h>
#else
#include <arpa/inet.h>
#define IN_ADDR in_addr
#define IN6_ADDR in6_addr
#endif // _WIN32

#include "KFC/Endian.h"
#include "KFC/OneOf.h"
#include "KFC/Preclude.h"
#include "KFC/Result.h"

namespace KFC {
enum class AddrParseError : int {
  Internal = 1,
  InvalidIPv4,
  InvalidIPv6,
};

class IPv6Addr {
public:
  explicit IPv6Addr() : m_octets() {}
  explicit IPv6Addr(const uint16 a, const uint16 b, const uint16 c, const uint16 d, const uint16 e,
                    const uint16 f, const uint16 g, const uint16 h)
      : m_octets() {
    KFC_CONSTEXPR_IF(KFC_LITTLE_ENDIAN) {
      const uint16 octets[8] = {swap(a), swap(b), swap(c), swap(d),
                                swap(e), swap(f), swap(g), swap(h)};
      std::memcpy(m_octets.addr16, octets, sizeof(octets));
    }
    else {
      const uint16 octets[8] = {a, b, c, d, e, f, g, h};
      std::memcpy(m_octets.addr16, octets, sizeof(octets));
    }
  }

  // ::/128
  KFC_NODISCARD bool isUnspecified() const {
    return m_octets.addr64[0] == 0 && m_octets.addr64[1] == 0;
  }

  // ::1/128
  KFC_NODISCARD bool isLoopback() const {
    return m_octets.addr64[0] == 0 && m_octets.addr64[1] == htonll(1);
  }

  KFC_NODISCARD bool isLinkLocal() const {
    return (m_octets.addr8[0] == 0xfe) && ((m_octets.addr8[1] & 0xc0) == 0x80);
  }

  // fc00::/7
  KFC_NODISCARD bool isUniqueLocal() const { return (m_octets.addr8[0] & 0xfe) == 0xfc; }

  // ff00::/8
  KFC_NODISCARD bool isMulticast() const { return m_octets.addr8[0] == 0xff; }

  // 2000::/3
  KFC_NODISCARD bool isGlobal() const { return (m_octets.addr8[0] & 0xe0) == 0x20; }

  KFC_NODISCARD String toString() const;

  KFC_NODISCARD bool operator==(const IPv6Addr &other) const {
    return m_octets.addr64[0] == other.m_octets.addr64[0] &&
           m_octets.addr64[1] == other.m_octets.addr64[1];
  }

  static Result<IPv6Addr, AddrParseError> parse(const String &str);
  static IPv6Addr fromInAddr(const IN6_ADDR &);

private:
  union {
    uint8_t addr8[16];
    uint16 addr16[8];
    uint32_t addr32[4];
    uint64_t addr64[2];
  } m_octets;
};

class IPv4Addr {
public:
  explicit IPv4Addr() : m_octets() {}
  explicit IPv4Addr(const uint8 a, const uint8 b, const uint8 c, const uint8 d) : m_octets() {
    m_octets[0] = a;
    m_octets[1] = b;
    m_octets[2] = c;
    m_octets[3] = d;
  }

  // 0.0.0.0/8
  KFC_NODISCARD bool isUnspecified() const { return m_octets[0] == 0; }
  // 127.0.0.0/8
  KFC_NODISCARD bool isLoopback() const { return m_octets[0] == 127; }
  // 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16
  KFC_NODISCARD bool isPrivate() const {
    return (m_octets[0] == 10) || (m_octets[0] == 172 && m_octets[1] >= 16 && m_octets[1] <= 31) ||
           (m_octets[0] == 192 && m_octets[1] == 168);
  }
  // 169.254.0.0/16
  KFC_NODISCARD bool isLinkLocal() const { return (m_octets[0] == 169 && m_octets[1] == 254); }
  // 224.0.0.0/4
  KFC_NODISCARD bool isMulticast() const { return m_octets[0] >= 224; }
  // 255.255.255.255/32
  KFC_NODISCARD bool isBroadcast() const { return m_octets[0] == 255; }
  // 240.0.0.0/4
  KFC_NODISCARD bool isReserved() const { return (m_octets[0] & 240) == 240 && !isBroadcast(); }

  KFC_NODISCARD String toString() const;

  KFC_NODISCARD uint8 operator[](const size_t index) const { return m_octets[index]; }

  KFC_NODISCARD IPv6Addr toIPv6Compatible() const {
    const uint16 g = (m_octets[1] << 8) | m_octets[0];
    const uint16 h = (m_octets[3] << 8) | m_octets[2];
    return IPv6Addr(0, 0, 0, 0, 0, 0, g, h);
  }

  KFC_NODISCARD IPv6Addr toIPv6Mapped() const {
    const uint16 g = (m_octets[1] << 8) | m_octets[0];
    const uint16 h = (m_octets[3] << 8) | m_octets[2];
    return IPv6Addr(0, 0, 0, 0, 0, 0xFFFF, g, h);
  }

  KFC_NODISCARD uint32 toUint32() const {
    return (m_octets[0] << 24) | (m_octets[1] << 16) | (m_octets[2] << 8) | m_octets[3];
  }

  static IPv4Addr fromUint32(const uint32 bits) {
    return IPv4Addr((bits >> 24) & 0xFF, (bits >> 16) & 0xFF, (bits >> 8) & 0xFF, bits & 0xFF);
  }

  static Result<IPv4Addr, AddrParseError> parse(const String &str);
  static IPv4Addr fromInAddr(const IN_ADDR &);

private:
  uint8 m_octets[4];
};

class IPAddr {
public:
  explicit IPAddr() : m_family(AF_UNSPEC) {}
  explicit IPAddr(const IPv4Addr &addr) : m_family(AF_INET) {
    KFC_DISCARD(m_addr.emplace<IPv4Addr>(addr));
  }
  explicit IPAddr(const IPv6Addr &addr) : m_family(AF_INET6) {
    KFC_DISCARD(m_addr.emplace<IPv6Addr>(addr));
  }

  IPAddr &operator=(const IPv4Addr &other) {
    m_family = AF_INET;
    KFC_DISCARD(m_addr.emplace<IPv4Addr>(other));
    return *this;
  }

  IPAddr &operator=(const IPv6Addr &other) {
    m_family = AF_INET6;
    KFC_DISCARD(m_addr.emplace<IPv6Addr>(other));
    return *this;
  }

  KFC_NODISCARD constexpr int isV4() const { return m_family == AF_INET; }
  KFC_NODISCARD constexpr int isV6() const { return m_family == AF_INET6; }
  KFC_NODISCARD String toString() const;

  template <class T> KFC_NODISCARD constexpr const T &as() const { return std::get<T>(m_addr); }

  static Result<IPAddr, AddrParseError> parse(const String &str);

private:
  int m_family;
  OneOf<IPv4Addr, IPv6Addr> m_addr;
};

struct SocketAddrV4 {
  IPv4Addr ip;
  uint16 port;
  SocketAddrV4() : port(0) {}
  SocketAddrV4(const IPv4Addr &ip, const uint16 port) : ip(ip), port(port) {}
};

struct SocketAddrV6 {
  IPv6Addr ip;
  uint16 port;
  SocketAddrV6() : port(0) {}
  SocketAddrV6(const IPv6Addr &ip, const uint16 port) : ip(ip), port(port) {}
};

class SocketAddr {
public:
  explicit SocketAddr() : m_family(AF_UNSPEC) {}
  explicit SocketAddr(const IPv4Addr &ip, const uint16 port) : m_family(AF_INET) {
    KFC_DISCARD(m_addr.emplace<SocketAddrV4>(SocketAddrV4(ip, port)));
  }
  explicit SocketAddr(const IPv6Addr &ip, const uint16 port) : m_family(AF_INET6) {
    KFC_DISCARD(m_addr.emplace<SocketAddrV6>(SocketAddrV6(ip, port)));
  }

  KFC_NODISCARD constexpr int isV4() const { return m_family == AF_INET; }
  KFC_NODISCARD constexpr int isV6() const { return m_family == AF_INET6; }
  KFC_NODISCARD String toString() const;

  template <class T> KFC_NODISCARD constexpr const T &as() const { return std::get<T>(m_addr); }

  static Result<SocketAddr, AddrParseError> parse(const String &str);

private:
  int m_family;
  OneOf<SocketAddrV4, SocketAddrV6> m_addr;
};
} // namespace KFC
