#pragma once
#include "TX/Platform.h"
#include "TX/RunLoop.h"
#include "TransportCore/Task/Task.h"

namespace TransportCore {
class TaskManager final : public TX::RunLoop::Timer {
 public:
  explicit TaskManager()
      : Timer(0, TX::Duration::Second(1), UINT64_MAX),
        thread_(TX::RunLoop::SpawnThread("TransportCoreMainRunLoop")),
        run_loop_(TX::RunLoop::FromThread(thread_->GetId())) {}

  void Start();
  void Stop();

  TX_NODISCARD std::string GetProxyURL(int32_t task_id);
  TX_NODISCARD int32_t CreateTask(const Task::Context &context);
  void StartTask(int32_t task_id);
  void StopTask(int32_t task_id);
  void PauseTask(int32_t task_id);
  void ResumeTask(int32_t task_id);
  void OnTimeout(TX::RunLoop &, TX::RefPtr<TX::RunLoop::Scope> &) override;

 private:
  TX::Option<Task> FindTask(int32_t task_id);

 private:
  struct Guard {
    std::unordered_map<int32_t, Task> task_map_;
  };

  TX::Time start_time_;
  TX::Mutex<Guard> guard_;
  TX::Own<TX::Thread> thread_;
  TX::Ref<TX::RunLoop> run_loop_;
};
}  // namespace TransportCore
