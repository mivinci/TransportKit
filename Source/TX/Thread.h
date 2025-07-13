#pragma once
#include <exception>
#include <functional>
#include <utility>

#include "TX/Assert.h"
#include "TX/Memory.h"
#include "TX/Own.h"
#include "TX/Platform.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>

#include <cstring>
#endif

namespace TX {
class TX_NODISCARD Thread final {
 public:
  using Func = std::function<void()>;
  explicit Thread(Func f, std::string name = "")
      : m_detached(false), m_func(std::move(f)), m_name(std::move(name)) {
#ifdef _WIN32
    m_handle =
        CreateThread(nullptr, 0, ThreadProc, this, CREATE_SUSPENDED, nullptr);
    if (!m_handle) TX_FATAL("CreateThread, error code %d", GetLastError());
    ResumeThread(m_handle);
#else
    TX_ASSERT_SYSCALL(pthread_create(&m_tid, nullptr, &Run, this));
#endif
  }

  ~Thread() {
    if (!m_detached) Join();
  }

  TX_DISALLOW_COPY(Thread)

  void Join() {
#ifdef _WIN32
    if (WaitForSingleObject(m_handle, INFINITE))
      TX_FATAL("WaitForSingleObject, error code %d", GetLastError());
    CloseHandle(m_handle);
    m_handle = nullptr;
#else
    TX_ASSERT_SYSCALL(pthread_join(m_tid, nullptr));
#endif
  }

  void Detach() {
#ifdef _WIN32
#else
    TX_ASSERT_SYSCALL(pthread_detach(m_tid));
#endif
    m_detached = true;
  }

  class Id {
#ifdef _WIN32
   public:
    explicit Id(HANDLE id) : m_id(id) {}
    bool operator<(const Id &other) const { return m_id < other.m_id; }
    bool operator==(const Id &other) const { return m_id == other.m_id; }

   private:
    HANDLE m_id;
#else

   public:
    explicit Id(const pthread_t id) : m_id(id) {}
    bool operator<(const Id &other) const { return m_id < other.m_id; }
    bool operator==(const Id &other) const { return m_id == other.m_id; }

   private:
    pthread_t m_id;
#endif
    friend Thread;
    friend std::hash<Id>;
  };

  TX_NODISCARD static Id Current() {
#ifdef _WIN32
    return Id(GetCurrentThread());
#else
    return Id(pthread_self());
#endif
  }

  TX_NODISCARD static Id Main() {
#ifdef _WIN32
    return Id(Thread::s_mainThreadId);
#else
    return Id(Thread::s_mainThreadId);
#endif
  }

  TX_NODISCARD Id GetId() const {
#ifdef _WIN32
    return Id(m_handle);
#else
    return Id(m_tid);
#endif
  }

  static Own<Thread> Spawn(Func f, const std::string &name = "") {
    return Own<Thread>(new Thread(std::move(f), name));
  }

 private:
  static void *Run(void *arg) {
    auto *t = static_cast<Thread *>(arg);
    if (!t->m_name.empty()) {
#ifdef _WIN32
      const HRESULT hr = SetThreadDescription(
          t->m_handle, reinterpret_cast<PCWSTR>(t->m_name.c_str()));
      TX_ASSERT(!FAILED(hr), "SetThreadDescription");
#elif defined(__APPLE__)
      TX_ASSERT_SYSCALL(pthread_setname_np(t->m_name.c_str()));
#elif defined(__linux__)
      char name[16];
      std::memcpy(name, t->m_name.c_str(),
                  std::min(t->m_name.size(), sizeof(name) - 1));
      name[sizeof(name) - 1] = '\0';
      TX_ASSERT_SYSCALL(pthread_setname_np(pthread_self(), name));
#else
#error "Unsupported platform for thread name setting"
#endif
    }

    try {
      t->m_func();
    } catch (...) {
      t->m_eptr = std::current_exception();
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
  bool m_detached;
  Func m_func;
  std::string m_name;
  std::exception_ptr m_eptr;
#ifdef _WIN32
  HANDLE m_handle;
  static HANDLE s_mainThreadId;
#else
  pthread_t m_tid{};
  static pthread_t s_mainThreadId;
#endif
};
}  // namespace TX

template <>
struct std::hash<TX::Thread::Id> {
  size_t operator()(const TX::Thread::Id &id) const noexcept {
    return reinterpret_cast<size_t>(id.m_id);
  }
};  // namespace std
