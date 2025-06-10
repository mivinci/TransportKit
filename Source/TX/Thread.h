#pragma once
#include <functional>
#include <utility>

#include "TX/Assert.h"
#include "TX/Memory.h"
#include "TX/Own.h"
#include "TX/Platform.h"
#include "TX/String.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace TX {
class TX_NODISCARD Thread final {
 public:
  using Func = std::function<void()>;
  explicit Thread(Func f, String name = "")
      : detached_(false), func_(std::move(f)), name_(std::move(name)) {
#ifdef _WIN32
    handle_ =
        CreateThread(nullptr, 0, ThreadProc, this, CREATE_SUSPENDED, nullptr);
    if (!handle_) TX_FATAL("CreateThread, error code {:d}", GetLastError());
    ResumeThread(handle_);
#else
    TX_ASSERT_SYSCALL(pthread_create(&tid_, nullptr, &Run, this));
#endif
  }

  ~Thread() {
    if (!detached_) Join();
  }

  TX_DISALLOW_COPY(Thread)

  void Join() const {
#ifdef _WIN32
    if (WaitForSingleObject(handle_, INFINITE))
      TX_FATAL("WaitForSingleObject, error code {:d}", GetLastError());
    CloseHandle(handle_);
    handle_ = nullptr;
#else
    TX_ASSERT_SYSCALL(pthread_join(tid_, nullptr));
#endif
  }

  void Detach() {
#ifdef _WIN32
#else
    TX_ASSERT_SYSCALL(pthread_detach(tid_));
#endif
    detached_ = true;
  }

  class Id {
#ifdef _WIN32
   public:
    explicit Id(const unsigned long id) : id_(id) {}
    bool operator<(const Id &other) const { return id_ < other.id_; }
    bool operator==(const Id &other) const { return id_ == other.id_; }

   private:
    DWORD id_;
#else

   public:
    explicit Id(const pthread_t id) : id_(id) {}
    bool operator<(const Id &other) const { return id_ < other.id_; }
    bool operator==(const Id &other) const { return id_ == other.id_; }

   private:
    pthread_t id_;
#endif
    friend Thread;
    friend std::hash<Id>;
  };

  TX_NODISCARD static Id Current() {
#ifdef _WIN32
    return Id(GetCurrentThreadId());
#else
    return Id(pthread_self());
#endif
  }

  TX_NODISCARD static Id Main() {
#ifdef _WIN32
    return Id(Thread::MainThreadId);
#else
    return Id(Thread::MainThreadId);
#endif
  }

  static Own<Thread> Spawn(Func f, const String &name = "") {
    return Own(new Thread(std::move(f), name));
  }

 private:
  static void *Run(void *arg) {
    auto *t = static_cast<Thread *>(arg);
    if (!t->name_.empty()) {
#ifdef _WIN32
      const HRESULT hr = SetThreadDescription(
          t->handle_, reinterpret_cast<PCWSTR>(t->name_.c_str()));
      TX_ASSERT(!FAILED(hr), "SetThreadDescription");
#elif defined(__APPLE__)
      TX_ASSERT_SYSCALL(pthread_setname_np(t->name_.c_str()));
#elif defined(__linux__)
      TX_ASSERT_SYSCALL(pthread_setname_np(pthread_self(), t->name_.c_str()));
#else
#endif
    }

    try {
      t->func_();
    } catch (...) {
      t->eptr_ = std::current_exception();
    }
    return nullptr;
  }

#ifdef _WIN32
  static DWORD WINAPI ThreadProc(LPVOID arg) {
    Run(arg);
    return 0;
  }
#endif

 private:
  bool detached_;
  Func func_;
  String name_;
  std::exception_ptr eptr_;
#ifdef _WIN32
  HANDLE handle_;
  static DWORD MainThreadId;
#else
  pthread_t tid_{};
  static pthread_t MainThreadId;
#endif
};

#ifdef _WIN32
inline DWORD Thread::MainThreadId = GetCurrentThreadId();
#else
inline pthread_t Thread::MainThreadId = pthread_self();
#endif
}  // namespace TX

template <>
struct std::hash<TX::Thread::Id> {
  size_t operator()(const TX::Thread::Id &id) const noexcept {
    return reinterpret_cast<size_t>(id.id_);
  }
};  // namespace std
