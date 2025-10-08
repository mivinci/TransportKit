#include "KFC/Assert.h"
#include "KFC/Unix/OwnFd.h"
#include <unistd.h>

KFC_NAMESPACE_BEG

OwnFd::~OwnFd() noexcept(false) {
  if (m_fd < 0) return;
  KFC_CHECK_SYSCALL(close(m_fd));
}

OwnFd &OwnFd::operator=(OwnFd &&other) noexcept {
  OwnFd move = std::move(*this);
  m_fd = other.m_fd;
  other.m_fd = -1;
  return *this;
}

int OwnFd::get() const { return m_fd; }
int OwnFd::take() {
  const int fd = m_fd;
  m_fd = -1;
  return fd;
}

KFC_NAMESPACE_END
