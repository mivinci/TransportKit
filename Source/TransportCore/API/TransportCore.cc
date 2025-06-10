#include "TransportCore/API/TransportCore.h"

#include "TransportCore/Task/TaskManager.h"

static TX::Mutex<TransportCore::TaskManager> gTaskManagerGuard;

void TransportCoreInit() {
  auto gTaskManager = gTaskManagerGuard.Lock();
  gTaskManager->Start();
}

void TransportCoreDestroy() {
  auto gTaskManager = gTaskManagerGuard.Lock();
  gTaskManager->Stop();
}

int32_t TransportCoreCreateTask(const TransportCoreTaskContext &context) {
  return gTaskManagerGuard.Lock()->CreateTask(context);
}

void TransportCoreStartTask(const int32_t task_id) {
  gTaskManagerGuard.Lock()->StartTask(task_id);
}

void TransportCoreStopTask(const int32_t task_id) {
  gTaskManagerGuard.Lock()->StopTask(task_id);
}

void TransportCorePauseTask(const int32_t task_id) {
  gTaskManagerGuard.Lock()->PauseTask(task_id);
}

void TransportCoreResumeTask(const int32_t task_id) {
  gTaskManagerGuard.Lock()->ResumeTask(task_id);
}

int64_t TransportCoreReadData(int32_t task_id, size_t offset, size_t size,
                              char *buf) {
  return 0;
}

void TransportCoreGetProxyURL(const int32_t task_id, char *buf,
                              const size_t buf_size) {
  const std::string url = gTaskManagerGuard.Lock()->GetProxyURL(task_id);
  const size_t size = std::min(buf_size, url.size());
  std::memcpy(buf, url.data(), size);
  buf[size - 1] = '\0';
}

void TransportCoreGetErrorString(TransportCoreErrorCode, char *, size_t) {}
