#pragma once
#include "TX/Clock.h"
#include "TX/Platform.h"
#include "TX/RunLoop.h"
#include "TX/Ticker.h"
#include "TransportCore/Global/Global.h"
#include "TransportCore/Task/Task.h"

namespace TransportCore {
constexpr int32_t kTaskIdBase = 100000;
constexpr int8_t kTaskIdSpan = 1;

class TaskManager {
 public:
  explicit TaskManager()
      : m_ticker(this, &TaskManager::OnSchedule, TX::Duration::Second(1),
                 "TaskManager", GetMainRunLoop()) {}

  TK_RESULT Start();
  TK_RESULT Stop();

  TX_NODISCARD std::string GetProxyURL(int32_t task_id);
  TX_NODISCARD int32_t CreateTask(const TransportCoreTaskContext &context);
  TK_RESULT StartTask(int32_t task_id);
  TK_RESULT StopTask(int32_t task_id);
  TK_RESULT PauseTask(int32_t task_id);
  TK_RESULT ResumeTask(int32_t task_id);
  int64_t ReadData(int32_t task_id, int32_t clip_no, size_t offset, size_t size,
                   char *buf);

  void OnSchedule(TX::Tick);
  TX_NODISCARD TX::Tick GetTick() const { return m_ticker.GetTick(); }

 private:
  TX::Option<Task> findTask(int32_t task_id);

 private:
  struct Guard {
    std::unordered_map<int32_t, Task> m_task_map;
  };

  TX::Time m_start_time;
  TX::Mutex<Guard> m_guard;
  TX::Ticker<TaskManager> m_ticker;

 public:
  class Id {
   public:
    static int32_t Next(const TransportCoreTaskKind kind, const int32_t start,
                        const int32_t span) noexcept {
      static std::atomic<int32_t> s_taskIdGen;
      const int32_t base = start * (static_cast<int32_t>(kind) + 1);
      const int32_t offset =
          s_taskIdGen.fetch_add(span, std::memory_order_relaxed);
      return offset + base;
    }

    static TransportCoreTaskKind GetKind(const int32_t task_id,
                                         const int32_t start) {
      return static_cast<TransportCoreTaskKind>(task_id / start);
    }
  };
};
}  // namespace TransportCore
