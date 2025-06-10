#include "TransportCore/Task/TaskManager.h"

#include "TX/Trace.h"
#include "TransportCore/Log/Log.h"

namespace TransportCore {
void TaskManager::Start() {
  TX_TRACE_START("TaskManager");
  start_time_ = TX::Time::Now();
  run_loop_->AddTimer(this);
}

void TaskManager::Stop() {
  run_loop_->RemoveTimer(this);
  auto guard = guard_.Lock();
  for (auto &it : guard->task_map_) {
    it.second.Stop();
  }
  TX_TRACE_END;
}

int32_t TaskManager::CreateTask(const Task::Context &context) {
  TX_TRACE_SPAN;
  const Task::Kind kind = context.kind;
  Task task(context, nullptr);
  const int32_t task_id = task.Id();
  auto guard = guard_.Lock();
  guard->task_map_.insert({task_id, std::move(task)});
  return task_id;
}

void TaskManager::StartTask(const int32_t task_id) {
  TX_IF_SOME(task, FindTask(task_id)) { task.Start(); }
  TX_TRACE_SPAN;
}

void TaskManager::StopTask(const int32_t task_id) {
  TX_IF_SOME(task, FindTask(task_id)) { task.Stop(); }
  TX_TRACE_SPAN;
}

void TaskManager::PauseTask(const int32_t task_id) {
  TX_IF_SOME(task, FindTask(task_id)) { task.Pause(); }
  TX_TRACE_SPAN;
}

void TaskManager::ResumeTask(const int32_t task_id) {
  TX_TRACE_SPAN;
  TX_IF_SOME(task, FindTask(task_id)) { task.Resume(); }
}

std::string TaskManager::GetProxyURL(const int32_t task_id) {
  TX_IF_SOME_CONST(task, FindTask(task_id)) { return task.GetProxyURL(); }
  TX_TRACE_SPAN;
  return "";
}

TX::Option<Task> TaskManager::FindTask(const int32_t task_id) {
  auto &task_map = guard_.Lock()->task_map_;
  const auto it = task_map.find(task_id);
  return it == task_map.end() ? TX::None : TX::Some(it->second);
}

void TaskManager::OnTimeout(TX::RunLoop &, TX::RefPtr<TX::RunLoop::Scope> &) {}

}  // namespace TransportCore
