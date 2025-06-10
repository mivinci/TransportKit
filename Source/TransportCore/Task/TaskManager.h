#pragma once
#include "TX/Assert.h"
#include "TX/Platform.h"
#include "TransportCore/Task/Task.h"

namespace TransportCore {
class TaskManager {
 public:
  explicit TaskManager(const int32_t base = kTaskIdBase,
                       const int8_t span = kTaskIdSpan)
      : base_(base), span_(span), run_loop_(TX::RunLoop::Current()) {
    TX_ASSERT(TX::IsPowerOfTen(base),
              "Task ID generator factor must be the power of 10");
  }

  void Start();
  void Stop();

  TX_NODISCARD std::string GetProxyURL(int32_t task_id);
  TX_NODISCARD int32_t CreateTask(const Task::Context &context);
  void StartTask(int32_t task_id);
  void StopTask(int32_t task_id);
  void PauseTask(int32_t task_id);
  void ResumeTask(int32_t task_id);

 private:
  TX::Option<Task> FindTask(int32_t task_id);

 private:
  struct Guard {
    std::unordered_map<int32_t, Task> task_map_;
  };

  const int32_t base_;
  const int32_t span_;
  TX::Time start_time_;
  TX::Ref<TX::RunLoop> run_loop_;
  TX::Mutex<Guard> guard_;
};
}  // namespace TransportCore
