#include "TransportCore/Task/TaskManager.h"

#include "TX/Runtime.h"
#include "TX/Trace.h"
#include "TransportCore/Log/Log.h"

namespace TransportCore {
void TaskManager::Start() {
  TX_TRACE_START("TaskManager");
  start_time_ = TX::Time::Now();
  TX::SpawnBlocking([this]() { run_loop_->Run(); });
}

void TaskManager::Stop() {
  auto guard = guard_.Lock();
  for (auto &it : guard->task_map_) {
    it.second.Stop();
  }
  run_loop_->Stop();
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
  TX_TRACE_SPAN;
  const auto task = FindTask(task_id);
  if (task.IsSome()) task.Unwrap().Start();
}

void TaskManager::StopTask(const int32_t task_id) {
  TX_TRACE_SPAN;
  const auto task = FindTask(task_id);
  if (task.IsSome()) task.Unwrap().Stop();
}

void TaskManager::PauseTask(const int32_t task_id) {
  TX_TRACE_SPAN;
  const auto task = FindTask(task_id);
  if (task.IsSome()) task.Unwrap().Pause();
}

void TaskManager::ResumeTask(const int32_t task_id) {
  TX_TRACE_SPAN;
  const auto task = FindTask(task_id);
  if (task.IsSome()) task.Unwrap().Resume();
}

std::string TaskManager::GetProxyURL(const int32_t task_id) {
  TX_TRACE_SPAN;
  const auto task = FindTask(task_id);
  if (task.IsSome()) return task.Unwrap().GetProxyURL();
  return "";
}

TX::Option<Task> TaskManager::FindTask(const int32_t task_id) {
  auto &task_map = guard_.Lock()->task_map_;
  const auto it = task_map.find(task_id);
  return it == task_map.end() ? TX::None : TX::Some(it->second);
}

}  // namespace TransportCore
