#include "KFC/Unix/Fd.h"

#include "KFC/Assert.h"

#include <unistd.h>

KFC_NAMESPACE_BEG

Fd::~Fd() noexcept(false) {
  if (m_fd < 0) return;
  KFC_CHECK_SYSCALL(close(m_fd));
}

Fd &Fd::operator=(Fd &&other) noexcept {
  Fd move = std::move(*this);
  m_fd = other.m_fd;
  other.m_fd = -1;
  return *this;
}

int Fd::get() const { return m_fd; }
int Fd::take() {
  const int fd = m_fd;
  m_fd = -1;
  return fd;
}

KFC_NAMESPACE_END
