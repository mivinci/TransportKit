#pragma once
#include "KFC/Clock.h"
#include "KFC/Mutex.h"
#include "KFC/Preclude.h"
#include "KFC/ScheduleHandle.h"
#include "KFC/Time.h"
#include "TransportCore/task/Task.h"

#include <unordered_map>

namespace TransportCore {

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
};

} // namespace TransportCore
