#pragma once
#include <atomic>

#include "TX/Bits.h"
#include "TX/Ref.h"

namespace TX {
class Task : public AtomicRefCounted<Task> {
 public:
  explicit Task() : id_(Id::Next()) {}
  explicit Task(const uint64_t id) : id_(id) {}
  virtual void Run() = 0;

  template <class R>
  class Handle {
    friend class BlockingPool;
    TX_DISALLOW_COPY(Handle)
    explicit Handle(const Ref<Task> &task) : task_(task) {}
    Ref<Task> task_;
  };

 private:
  class Id {
    friend Task;
    static uint64 Next() {
      static std::atomic<uint64> id = 0;
      return id.fetch_add(1, std::memory_order_relaxed);
    }
  };

  uint64_t id_;
};
}  // namespace TX
