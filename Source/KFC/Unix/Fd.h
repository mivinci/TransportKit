#pragma once

#include "KFC/CopyMove.h"
#include "KFC/Preclude.h"

KFC_NAMESPACE_BEG

// `Fd` closes the underlying file descriptor when it leaves out of scope. KJ calls this
// `AutoCloseFd` but I guess `Fd` is intuitive enough.
class Fd {
public:
  KFC_DISALLOW_COPY(Fd);
  constexpr Fd() : Fd(-1) {};
  constexpr Fd(const int fd) : m_fd(fd) {};
  constexpr Fd(Fd &&other) noexcept : m_fd(other.m_fd) { other.m_fd = -1; }
  ~Fd() noexcept(false);

  Fd &operator=(Fd &&other) noexcept;

  // ReSharper disable once CppNonExplicitConversionOperator
  constexpr operator int() const { return m_fd; }
  KFC_NODISCARD int get() const;
  KFC_NODISCARD int take();

private:
  int m_fd;
};

KFC_NAMESPACE_END
