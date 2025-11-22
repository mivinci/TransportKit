#pragma once
#include "KFC/Clock.h"
#include "KFC/Mutex.h"
#include "KFC/Preclude.h"
#include "KFC/ScheduleHandle.h"
#include "KFC/Time.h"
#include "TransportCore/task/Task.h"

#include <unordered_map>

namespace TransportCore {
constexpr int32_t kTaskIdBase = 100000;
constexpr int8_t kTaskIdSpan = 1;

class TaskManager {
public:
  explicit TaskManager()
      : m_scheduleHandle(this, &TaskManager::OnSchedule, KFC::Duration::fromSecond(1)) {}

  TK_RESULT Start();
  TK_RESULT Stop();

  KFC_NODISCARD std::string GetProxyURL(int32_t task_id);
  KFC_NODISCARD int32_t CreateTask(const TransportCoreTaskContext &context);
  TK_RESULT StartTask(int32_t task_id);
  TK_RESULT StopTask(int32_t task_id);
  TK_RESULT PauseTask(int32_t task_id);
  TK_RESULT ResumeTask(int32_t task_id);
  int64_t ReadData(int32_t task_id, int32_t clip_no, size_t offset, size_t size, char *buf);

  void OnSchedule(KFC::Tick);

private:
  KFC::Option<Task> findTask(int32_t task_id);

  struct Guard {
    std::unordered_map<int32_t, Task> m_taskMap;
  };

  KFC::Time m_startTime;
  KFC::Mutex<Guard> m_guard;
  KFC::ScheduleHandle<TaskManager> m_scheduleHandle;

public:
  class TaskId {
  public:
    static int32_t Next(const TransportCoreTaskKind kind, const int32_t start,
                        const int32_t span) noexcept {
      static std::atomic<int32_t> s_taskIdGen;
      const int32_t base = start * (static_cast<int32_t>(kind) + 1);
      const int32_t offset = s_taskIdGen.fetch_add(span, std::memory_order_relaxed);
      return offset + base;
    }

    static TransportCoreTaskKind GetKind(const int32_t task_id, const int32_t start) {
      return static_cast<TransportCoreTaskKind>(task_id / start);
    }
  };
};
} // namespace TransportCore
