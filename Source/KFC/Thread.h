#pragma once

#include "KFC/CopyMove.h"
#include "KFC/Option.h"
#include "KFC/Own.h"
#include "KFC/Preclude.h"
#include <functional>

KFC_NAMESPACE_BEG
class Thread;
class ThreadDisposer final : public DeleteStaticDisposer<Thread> {};
using OwnThread = Own<Thread, ThreadDisposer>;

class KFC_NODISCARD Thread final {
public:
  KFC_DISALLOW_COPY(Thread)
  using Func = std::function<void()>;

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

  explicit Thread(Func &&func, std::string name = "");
  ~Thread() noexcept(false);

  template <class Func> static OwnThread spawn(Func &&func, const std::string &name = "") {
    return new Thread(std::forward<Func>(func), name);
  }

  KFC_NODISCARD static Id current();
  void detach();

private:
  static void *run(void *arg);
  void join();

#ifdef _WIN32
  static DWORD WINAPI ThreadProc(LPVOID arg) {
    run(arg);
    return 0;
  }
#endif

  bool m_detached;
  Func m_func;
  Option<Exception> m_exception;
  String m_name;

#ifdef _WIN32
  HANDLE m_handle;
#else
  pthread_t m_tid{};
#endif
};

void setCurrentThreadName(const String &name);
String getCurrentThreadName();

KFC_NAMESPACE_END

template <> struct std::hash<KFC_NAMESPACE::Thread::Id> {
  size_t operator()(const KFC_NAMESPACE::Thread::Id &id) const noexcept {
    return reinterpret_cast<size_t>(id.m_id);
  }
};
