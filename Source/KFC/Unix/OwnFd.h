#pragma once

#include "KFC/CopyMove.h"
#include "KFC/Preclude.h"

KFC_NAMESPACE_BEG

// `OwnFd` closes the underlying file descriptor when leaving out of scope.
class OwnFd {
public:
  KFC_DISALLOW_COPY(OwnFd);
  constexpr OwnFd() : OwnFd(-1) {};
  constexpr OwnFd(const int fd) : m_fd(fd) {};
  constexpr OwnFd(OwnFd &&other) noexcept : m_fd(other.m_fd) { other.m_fd = -1; }
  ~OwnFd() noexcept(false);

  OwnFd &operator=(OwnFd &&other) noexcept;

  // ReSharper disable once CppNonExplicitConversionOperator
  constexpr operator int() const { return m_fd; }
  KFC_NODISCARD int get() const;
  KFC_NODISCARD int take();

private:
  int m_fd;
};

KFC_NAMESPACE_END
