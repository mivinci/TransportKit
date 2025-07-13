#pragma once
#include <exception>
#include <functional>
#include <utility>

#include "KFC/Assert.h"
#include "KFC/Memory.h"
#include "KFC/Own.h"
#include "KFC/Platform.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>

#include <cstring>
#endif

namespace KFC {
class KFC_NODISCARD Thread final {
 public:
  using Func = std::function<void()>;
  explicit Thread(Func f, String name = "")
      : m_detached(false), m_func(std::move(f)), m_name(std::move(name)) {
#ifdef _WIN32
    m_handle = CreateThread(nullptr, 0, ThreadProc, this, CREATE_SUSPENDED, nullptr);
    if (!m_handle) KFC_FATAL("CreateThread, error code %d", GetLastError());
    ResumeThread(m_handle);
#else
    KFC_ASSERT_SYSCALL(pthread_create(&m_tid, nullptr, &run, this));
#endif
  }

  ~Thread() {
    if (!m_detached) join();
  }

  KFC_DISALLOW_COPY(Thread)

  void join() const {
#ifdef _WIN32
    if (WaitForSingleObject(m_handle, INFINITE))
      KFC_FATAL("WaitForSingleObject, error code %d", GetLastError());
    CloseHandle(m_handle);
    m_handle = nullptr;
#else
    KFC_ASSERT_SYSCALL(pthread_join(m_tid, nullptr));
#endif
  }

  void detach() {
#ifdef _WIN32
#else
    KFC_ASSERT_SYSCALL(pthread_detach(m_tid));
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

  KFC_NODISCARD static Id current() {
#ifdef _WIN32
    return Id(GetCurrentThread());
#else
    return Id(pthread_self());
#endif
  }

  KFC_NODISCARD static Id main() {
#ifdef _WIN32
    return Id(Thread::s_mainThreadId);
#else
    return Id(Thread::s_mainThreadId);
#endif
  }

  KFC_NODISCARD Id getId() const {
#ifdef _WIN32
    return Id(m_handle);
#else
    return Id(m_tid);
#endif
  }

  static Own<Thread> spawn(Func f, const String &name = "") {
    return Own<Thread>(new Thread(std::move(f), name));
  }

 private:
  static void *run(void *arg) {
    auto *t = static_cast<Thread *>(arg);
    if (!t->m_name.empty()) {
#ifdef _WIN32
      const HRESULT hr =
          SetThreadDescription(t->m_handle, reinterpret_cast<PCWSTR>(t->m_name.c_str()));
      KFC_ASSERT(!FAILED(hr), "SetThreadDescription");
#elif defined(__APPLE__)
      KFC_ASSERT_SYSCALL(pthread_setname_np(t->m_name.c_str()));
#elif defined(__linux__)
      char name[16];
      std::memcpy(name, t->m_name.c_str(), std::min(t->m_name.size(), sizeof(name) - 1));
      name[sizeof(name) - 1] = '\0';
      KFC_ASSERT_SYSCALL(pthread_setname_np(pthread_self(), name));
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

  bool m_detached;
  Func m_func;
  String m_name;
  std::exception_ptr m_eptr;
#ifdef _WIN32
  HANDLE m_handle;
  static HANDLE s_mainThreadId;
#else
  pthread_t m_tid{};
  static pthread_t s_mainThreadId;
#endif
};
}  // namespace KFC

template <>
struct std::hash<KFC::Thread::Id> {
  size_t operator()(const KFC::Thread::Id &id) const noexcept {
    return reinterpret_cast<size_t>(id.m_id);
  }
};  // namespace std
