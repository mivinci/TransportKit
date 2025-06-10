#pragma once
#include <coroutine>
#include <atomic>

#include "TX/Own.h"
#include "TX/Bits.h"

namespace TX {
class Task {
 public:
  virtual ~Task() = default;
  explicit Task() : id_(Id::Next()) {}
  explicit Task(uint64_t id) : id_(id) {}
  virtual void Run() = 0;

  template <class R>
  class Handle {
   public:
    template <class PromiseType>
    auto await_suspend(std::coroutine_handle<PromiseType> handle) {}
    bool await_ready() { return false; }
    R await_resume() { TX_UNREACHABLE(); }

   private:
    friend class BlockingPool;
    explicit Handle(Task *task) : task_(task){};
    TX_DISALLOW_COPY(Handle)
    UnOwn<Task> task_;
  };

  class Id {
   public:
    static uint64 Next() {
      static std::atomic<uint64> id = 0;
      return id.fetch_add(1, std::memory_order_relaxed);
    }
  };

 private:
  uint64 id_;
};
}  // namespace TX
