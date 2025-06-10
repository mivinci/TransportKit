#pragma once
#include "TX/Addr.h"
#include "TX/Platform.h"
#include "TX/Span.h"
#include "TX/net/Error.h"

namespace TX {
class UdpSocket {
 public:
  static Result<UdpSocket, NetError> Bind(const String &);
  Result<void, NetError> Connect(const String &);
  Result<int64, NetError> Read(Span<uint8> &buf);
  Result<int64, NetError> Write(const Span<uint8> &buf);
  TX_NODISCARD SocketAddr LocalAddr() const;
  TX_NODISCARD SocketAddr RemoteAddr() const;
};
}  // namespace TX
