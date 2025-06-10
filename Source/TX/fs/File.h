#pragma once
#if defined (_WIN32)
#else
#include <fcntl.h>
#endif

#include "TX/Bits.h"
#include "TX/Path.h"
#include "TX/Result.h"
#include "TX/String.h"
#include "TX/io/Error.h"
#include "TX/runtime/Runtime.h"

namespace TX {
#define TX_O_RDONLY O_RDONLY
#define TX_O_WRONLY O_WRONLY
#define TX_O_RDWR O_RDWR

enum class FileMode {};

class File {
 public:
  static Async<Result<File, IOError>> Open(const Path &path, int flag, uint32 mode) {
    return TX_ASYNCIFY([&]() { return OpenSync(path, flag, mode); });
  }

  static Result<File, IOError> OpenSync(const Path &path, int flag, uint32 mode) {
    #if defined (_WIN32)
    #else
    return File(::open(path.ToCStr(), static_cast<int>(flag), mode));
    #endif
  }

  explicit File() : fd_(-1){};
  explicit File(int fd) : fd_(fd) {}

 private:
  int fd_;
};
}  // namespace TX
