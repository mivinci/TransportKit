#pragma once
#include "TX/Platform.h"
#include "TransportCore/API/TransportCore.h"
#include "TransportCore/Task/TaskScheduler.h"

namespace TransportCore {
constexpr int32_t kTaskIdBase = 100000;
constexpr int8_t kTaskIdSpan = 1;

class Task {
 public:
  using Kind = TransportCoreTaskKind;
  using Event = TransportCoreTaskEvent;
  using Context = TransportCoreTaskContext;

  explicit Task(const Context &context, const TX::Ref<TX::RunLoop> &run_loop)
      : id_(Id::Next(context.kind, kTaskIdBase, kTaskIdSpan)),
        context_(context),
        scheduler_(run_loop) {}

  TX_NODISCARD int32_t Id() const { return id_; }
  TX_NODISCARD std::string GetProxyURL() const;

  void Start();
  void Stop();
  void Pause();
  void Resume();

 private:
  int32_t id_;
  Context context_;
  TX::Ref<TaskScheduler> scheduler_;

 public:
  class Id {
   public:
    static int32_t Next(const Kind kind, const int32_t start,
                        const int32_t span) noexcept {
      static std::atomic task_id_gen = 0;
      const int32_t base = start * static_cast<int32_t>(kind);
      const int32_t offset =
          task_id_gen.fetch_add(span, std::memory_order_relaxed);
      return (offset >= span) ? -1 : offset + base;
    }

    static Kind GetKind(const int32_t task_id, const int32_t start) {
      return static_cast<Kind>(task_id / start);
    }
  };
};
}  // namespace TransportCore
