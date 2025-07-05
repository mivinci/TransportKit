#include "TransportCore/Schedule/TaskManager.h"

#include "TX/Trace.h"
#include "TransportCore/Log/Log.h"

namespace TransportCore {
void TaskManager::Start() {
  start_time_ = TX::Time::Now();
  run_loop_->AddTimer(this);
}

void TaskManager::Stop() {
  auto guard = guard_.Lock();
  for (auto &it : guard->task_map_) {
    it.second.Stop();
  }
  run_loop_->RemoveTimer(this);
}

int32_t TaskManager::CreateTask(const TransportCoreTaskContext &context) {
  if (context.kind < kTransportCoreTaskKindUnSpec) return -1;

  int32_t task_id = Id::Next(context.kind, kTaskIdBase, kTaskIdSpan);
  if (task_id < 0) return -1;

  Task task(run_loop_, task_id, context);
  auto guard = guard_.Lock();
  guard->task_map_.insert({task_id, task});
  return task_id;
}

bool TaskManager::StartTask(const int32_t task_id) {
  TX_IF_SOME(task, findTask(task_id)) { return task.Start(); }
  return false;
}

bool TaskManager::StopTask(const int32_t task_id) {
  TX_IF_SOME(task, findTask(task_id)) { return task.Stop(); }
  return false;
}

bool TaskManager::PauseTask(const int32_t task_id) {
  TX_IF_SOME(task, findTask(task_id)) { return task.Pause(); }
  return false;
}

bool TaskManager::ResumeTask(const int32_t task_id) {
  TX_IF_SOME(task, findTask(task_id)) { return task.Resume(); }
  return false;
}

int64_t TaskManager::ReadData(int32_t task_id, int32_t clip_no, size_t offset,
                              size_t size, char *buf) {
  TX_IF_SOME(task, findTask(task_id)) {
    return task.ReadData(clip_no, offset, size, buf);
  }
  return -1;
}

std::string TaskManager::GetProxyURL(const int32_t task_id) {
  TX_IF_SOME(task, findTask(task_id)) { return task.GetProxyURL(); }
  return "";
}

void TaskManager::OnTimeout(TX::RunLoop &, TX::RefPtr<TX::RunLoop::Scope> &) {
  TK_INFO("schedule %d", GetTick());
}

TX::Option<Task> TaskManager::findTask(const int32_t task_id) {
  auto &task_map = guard_.Lock()->task_map_;
  const auto it = task_map.find(task_id);
  return it == task_map.end() ? TX::None : TX::Some(it->second);
}
}  // namespace TransportCore
