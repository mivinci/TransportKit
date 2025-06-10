#pragma once
#include "TX/Assert.h"
#include "TX/Endian.h"
#include "TX/Platform.h"
#include "TX/Result.h"
#include "TX/String.h"

namespace TX {
class AddrParseError {
 public:
  AddrParseError() = default;
  AddrParseError(const AddrParseError &) = default;
  AddrParseError(AddrParseError &&) = default;
  AddrParseError &operator=(const AddrParseError &) = default;
  AddrParseError &operator=(AddrParseError &&) = default;
  ~AddrParseError() = default;

  TX_NODISCARD bool operator==(const AddrParseError &) const { return true; }
};

class IPv6Addr {
 public:
  explicit IPv6Addr() = default;
  explicit IPv6Addr(const uint16 a, const uint16 b, const uint16 c,
                    const uint16 d, const uint16 e, const uint16 f,
                    const uint16 g, const uint16 h) {
    TX_CONSTEXPR_IF (TX_LITTLE_ENDIAN) {
      const uint16 octets[8] = {swap(a), swap(b), swap(c), swap(d),
                                swap(e), swap(f), swap(g), swap(h)};
      std::memcpy(octets_, octets, sizeof(octets));
    } else {
      const uint16 octets[8] = {a, b, c, d, e, f, g, h};
      std::memcpy(octets_, octets, sizeof(octets));
    }
  }

  TX_NODISCARD uint128 ToBits() const {
    return uint128::FromBigEndianBytes(octets_);
  }

  static IPv6Addr FromBits(const uint128 bits) {
    IPv6Addr ip{};
    bits.ToBigEndianBytes(ip.octets_);
    return ip;
  }

  static Result<IPv6Addr, AddrParseError> Parse(const String &addr);

 private:
  uint8 octets_[16]{};
};

class IPv4Addr {
 public:
  explicit IPv4Addr() = default;
  explicit IPv4Addr(const uint8 a, const uint8 b, const uint8 c,
                    const uint8 d) {
    octets_[0] = a;
    octets_[1] = b;
    octets_[2] = c;
    octets_[3] = d;
  }

  TX_NODISCARD bool IsLoopBack() const { return octets_[0] == 127; }
  TX_NODISCARD bool IsPrivate() const {
    return (octets_[0] == 10) ||
           (octets_[0] == 172 && octets_[1] >= 16 && octets_[1] <= 31) ||
           (octets_[0] == 192 && octets_[1] == 168);
  }
  TX_NODISCARD bool IsLinkLocal() const {
    return (octets_[0] == 169 && octets_[1] == 254);
  }
  TX_NODISCARD bool IsMulticast() const { return octets_[0] >= 224; }
  TX_NODISCARD bool IsBroadcast() const { return octets_[0] == 255; }
  TX_NODISCARD bool IsUnspecified() const { return octets_[0] == 0; }
  TX_NODISCARD bool IsReserved() const {
    return (octets_[0] & 240) == 240 && !IsBroadcast();
  }

  TX_NODISCARD uint8 operator[](const size_t index) const {
    return octets_[index];
  }

  TX_NODISCARD IPv6Addr ToIPv6Compatible() const {
    const uint16 g = (octets_[0] << 8) | octets_[1];
    const uint16 h = (octets_[2] << 8) | octets_[3];
    return IPv6Addr(0, 0, 0, 0, 0, 0, g, h);
  }

  TX_NODISCARD IPv6Addr ToIPv6Mapped() const {
    const uint16 g = (octets_[0] << 8) | octets_[1];
    const uint16 h = (octets_[2] << 8) | octets_[3];
    return IPv6Addr(0, 0, 0, 0, 0xFF, 0xFF, g, h);
  }

  TX_NODISCARD uint32 ToBits() const {
    return (octets_[0] << 24) | (octets_[1] << 16) | (octets_[2] << 8) |
           octets_[3];
  }

  static IPv4Addr FromBits(const uint32 bits) {
    return IPv4Addr((bits >> 24) & 0xFF, (bits >> 16) & 0xFF,
                    (bits >> 8) & 0xFF, bits & 0xFF);
  }

  static Result<IPv4Addr, AddrParseError> Parse(const String &addr);

 private:
  uint8 octets_[4]{};
};

struct SocketAddrV4 {
  IPv4Addr ip;
  uint16 port;
  SocketAddrV4() : port(0) {}
};

struct SocketAddrV6 {
  IPv6Addr ip;
  uint16 port;
  SocketAddrV6() : port(0) {}
};

class SocketAddr {
 public:
  explicit SocketAddr(const IPv4Addr &ip, const uint16 port)
      : is_v4_(true), addr_({}) {
    addr_.v4_.ip = ip;
    addr_.v4_.port = port;
  }
  explicit SocketAddr(const IPv6Addr &ip, const uint16 port)
      : is_v4_(false), addr_({}) {
    addr_.v6_.ip = ip;
    addr_.v6_.port = port;
  }

  static Result<SocketAddr, AddrParseError> Parse(const String &addr);

 private:
  bool is_v4_;
  union {
    SocketAddrV4 v4_;
    SocketAddrV6 v6_;
  } addr_;
};
}  // namespace TX
