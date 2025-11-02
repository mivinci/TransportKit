#include "TransportCore/Task/TaskManager.h"

#include "KFC/Trace.h"

namespace TransportCore {
TK_RESULT TaskManager::Start() {
  m_startTime = KFC::Time::now();
  m_scheduleHandle.start();
  return TK_OK;
}

TK_RESULT TaskManager::Stop() {
  auto guard = m_guard.lock();
  for (auto &it : guard->m_taskMap) {
    it.second.Stop();
  }
  m_scheduleHandle.stop();
  return TK_OK;
}

int32_t TaskManager::CreateTask(const TransportCoreTaskContext &context) {
  if (context.kind < kTransportCoreTaskKindUnSpec) return -1;

  int32_t task_id = TaskId::Next(context.kind, kTaskIdBase, kTaskIdSpan);
  if (task_id < 0) return -1;

  Task task(task_id, context);
  auto guard = m_guard.lock();
  guard->m_taskMap.insert({task_id, task});
  return task_id;
}

TK_RESULT TaskManager::StartTask(const int32_t task_id) {
  KFC_IF_SOME(task, findTask(task_id)) { return task.Start(); }
  return TK_OK;
}

TK_RESULT TaskManager::StopTask(const int32_t task_id) {
  KFC_IF_SOME(task, findTask(task_id)) { return task.Stop(); }
  return TK_OK;
}

TK_RESULT TaskManager::PauseTask(const int32_t task_id) {
  KFC_IF_SOME(task, findTask(task_id)) { return task.Pause(); }
  return TK_OK;
}

TK_RESULT TaskManager::ResumeTask(const int32_t task_id) {
  KFC_IF_SOME(task, findTask(task_id)) { return task.Resume(); }
  return TK_OK;
}

int64_t TaskManager::ReadData(const int32_t task_id, const int32_t clip_no,
                              const size_t offset, const size_t size,
                              char *buf) {
  KFC_IF_SOME(task, findTask(task_id)) {
    return task.ReadData(clip_no, offset, size, buf);
  }
  return TK_ERR;
}

std::string TaskManager::GetProxyURL(const int32_t task_id) {
  KFC_IF_SOME(task, findTask(task_id)) { return task.GetProxyURL(); }
  return "";
}

void TaskManager::OnSchedule(const KFC::Tick tick) {
  // We should do some non-task stuff here.
  // TK_INFO("schedule %d", tick);
}

KFC::Option<Task> TaskManager::findTask(const int32_t task_id) {
  auto &task_map = m_guard.lock()->m_taskMap;
  const auto it = task_map.find(task_id);
  return it == task_map.end() ? KFC::None : KFC::Some(it->second);
}
}  // namespace TransportCore
