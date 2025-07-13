#pragma once
#include <sys/socket.h>

#include "TX/RunLoop.h"

namespace TX {
class Socket final : public RunLoop::Source, public RefCounted<Socket> {
 public:
  Ref<Socket> Create(const int family, const int type, const int protocol) {
    const int fd = socket(family, type, protocol);

  }

 private:
  explicit Socket(const int fd) : fd_(fd) {}

  int fd_;
};
}  // namespace TX
