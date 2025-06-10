#include "TransportCore/API/TransportCore.h"

#include "TransportCore/Task/TaskManager.h"

static TX::Mutex<TransportCore::TaskManager> gTaskManagerLock;

void TransportCoreInit() {
  auto gTaskManager = gTaskManagerLock.Lock();
  gTaskManager->Start();
}

void TransportCoreDestroy() {
  auto gTaskManager = gTaskManagerLock.Lock();
  gTaskManager->Stop();
}

int32_t TransportCoreCreateTask(TransportCoreTaskContext context) {
  return gTaskManagerLock.Lock()->CreateTask(context);
}

void TransportCoreStartTask(const int32_t task_id) {
  gTaskManagerLock.Lock()->StartTask(task_id);
}

void TransportCoreStopTask(const int32_t task_id) {
  gTaskManagerLock.Lock()->StopTask(task_id);
}

void TransportCorePauseTask(const int32_t task_id) {
  gTaskManagerLock.Lock()->PauseTask(task_id);
}

void TransportCoreResumeTask(const int32_t task_id) {
  gTaskManagerLock.Lock()->ResumeTask(task_id);
}

int64_t TransportCoreReadData(int32_t task_id, size_t offset, size_t size,
                              char *buf, size_t buf_size) {
  return 0;
}

void TransportCoreGetProxyURL(const int32_t task_id, char *buf, size_t size) {
  const std::string url = gTaskManagerLock.Lock()->GetProxyURL(task_id);
  ::memcpy(buf, url.data(), std::min(size, url.size()));
}

void TransportCoreGetErrorString(TransportCoreErrorCode, char *, size_t) {}
