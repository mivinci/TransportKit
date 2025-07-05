#pragma once
#include "TX/Platform.h"
#include "TX/RunLoop.h"
#include "TransportCore/Schedule/Task.h"

namespace TransportCore {
constexpr int32_t kTaskIdBase = 100000;
constexpr int8_t kTaskIdSpan = 1;

class TaskManager final : public TX::RunLoop::Timer {
 public:
  explicit TaskManager(const TX::Ref<TX::RunLoop> &run_loop)
      : Timer(0, TX::Duration::Second(1), kTimerRepeatAlways, "TaskManager"),
        run_loop_(run_loop) {}

  void Start();
  void Stop();

  TX_NODISCARD std::string GetProxyURL(int32_t task_id);
  TX_NODISCARD int32_t CreateTask(const TransportCoreTaskContext &context);
  bool StartTask(int32_t task_id);
  bool StopTask(int32_t task_id);
  bool PauseTask(int32_t task_id);
  bool ResumeTask(int32_t task_id);
  int64_t ReadData(int32_t task_id, int32_t clip_no, size_t offset, size_t size,
                   char *buf);

  void OnTimeout(TX::RunLoop &, TX::RefPtr<TX::RunLoop::Scope> &) override;

 private:
  TX::Option<Task> findTask(int32_t task_id);

 private:
  struct Guard {
    std::unordered_map<int32_t, Task> task_map_;
  };

  TX::Time start_time_;
  TX::Mutex<Guard> guard_;
  TX::Ref<TX::RunLoop> run_loop_;

 public:
  class Id {
   public:
    static int32_t Next(const TransportCoreTaskKind kind, const int32_t start,
                        const int32_t span) noexcept {
      static std::atomic task_id_gen = 0;
      const int32_t base = start * (static_cast<int32_t>(kind) + 1);
      const int32_t offset =
          task_id_gen.fetch_add(span, std::memory_order_relaxed);
      return offset + base;
    }

    static TransportCoreTaskKind GetKind(const int32_t task_id,
                                         const int32_t start) {
      return static_cast<TransportCoreTaskKind>(task_id / start);
    }
  };
};
}  // namespace TransportCore
