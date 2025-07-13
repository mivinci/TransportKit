#include "KFC/Thread.h"

#include "KFC/Assert.h"
#include "KFC/RunCatchingExceptions.h"
#include <utility>

KFC_NAMESPACE_BEG

Thread::Thread(Func &&func, std::string name)
    : m_detached(false), m_func(std::move(func)), m_name(std::move(name)) {
#ifdef _WIN32
  m_handle = CreateThread(nullptr, 0, ThreadProc, this, CREATE_SUSPENDED, nullptr);
  KFC_CHECK(m_handle, "CreateThread, error code %d", GetLastError());
  ResumeThread(m_handle);
#else
  KFC_CHECK_SYSCALL(pthread_create(&m_tid, nullptr, &run, this));
#endif
}

Thread::~Thread() noexcept(false) {
  if (!m_detached) join();
}

void Thread::join() {
#ifdef _WIN32
  if (WaitForSingleObject(m_handle, INFINITE))
    KFC_THROW("WaitForSingleObject, error code %d", GetLastError());
  CloseHandle(m_handle);
  m_handle = nullptr;
#else
  KFC_CHECK_SYSCALL(pthread_join(m_tid, nullptr));
#endif
  KFC_IF_SOME(e, m_exception) { throw std::move(e); }
}

void Thread::detach() {
#ifdef _WIN32
#else
  KFC_CHECK_SYSCALL(pthread_detach(m_tid));
#endif
  m_detached = true;
}

void *Thread::run(void *arg) {
  auto *t = static_cast<Thread *>(arg);
  setCurrentThreadName(t->m_name);
  t->m_exception = runCatchingExceptions([&] { t->m_func(); });
  return nullptr;
}

Thread::Id Thread::current() {
#ifdef _WIN32
  return Id(GetCurrentThread());
#else
  return Id(pthread_self());
#endif
}

void setCurrentThreadName(const std::string &name) {
  if (!name.empty()) {
#ifdef _WIN32
    const HRESULT hr =
        SetThreadDescription(GetCurrentThread(), reinterpret_cast<PCWSTR>(name.c_str()));
    KFC_CHECK(!FAILED(hr), "SetThreadDescription");
#elif defined(__APPLE__)
    KFC_CHECK_SYSCALL(pthread_setname_np(name.c_str()));
#elif defined(__linux__)
    char buf[16];
    std::memcpy(buf, name.c_str(), std::min(name.size(), sizeof(buf) - 1));
    buf[sizeof(buf) - 1] = '\0';
    KFC_CHECK_SYSCALL(pthread_setname_np(pthread_self(), buf));
#else
#error "Unsupported platform for thread name setting"
#endif
  }
}

String getCurrentThreadName() {
  String name;
#ifdef _WIN32
  PWSTR desc;
  const HRESULT hr = GetThreadDescription(GetCurrentThread(), &desc);
  if (SUCCEEDED(hr)) {
    name = desc;
    LocalFree(desc);
  }
#elif defined(__APPLE__)
  char buf[64];
  KFC_CHECK_SYSCALL(pthread_getname_np(pthread_self(), buf, sizeof(buf)));
  name = buf;
#elif defined(__linux__)
  char buf[16];
  KFC_CHECK_SYSCALL(pthread_getname_np(pthread_self(), buf, sizeof(buf)));
  name = buf;
#else
#error "Unsupported platform for thread name getting"
#endif
  return name;
}

KFC_NAMESPACE_END
