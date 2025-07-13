#include "TransportCore/API/TransportCore.h"

#include <cstring>

#include "TransportCore/Task/TaskManager.h"

static TransportCore::TaskManager *g_taskManager = nullptr;
static KFC::Mutex<bool> g_inited;

void TransportCoreInit() {
  auto inited = g_inited.lock();
  if (*inited) return;
  g_taskManager = new TransportCore::TaskManager;
  g_taskManager->Start();
  *inited = true;
}

void TransportCoreDestroy() {
  auto inited = g_inited.lock();
  if (!*inited) return;
  g_taskManager->Stop();
  *inited = false;
  delete g_taskManager;
}

int32_t TransportCoreCreateTask(TransportCoreTaskContext context) {
  if (!*g_inited.lock()) return -1;
  return g_taskManager->CreateTask(context);
}

TK_RESULT TransportCoreStartTask(const int32_t task_id) {
  if (!*g_inited.lock()) return -1;
  return g_taskManager->StartTask(task_id);
}

TK_RESULT TransportCoreStopTask(const int32_t task_id) {
  if (!*g_inited.lock()) return -1;
  return g_taskManager->StopTask(task_id);
}

TK_RESULT TransportCorePauseTask(const int32_t task_id) {
  if (!*g_inited.lock()) return -1;
  return g_taskManager->PauseTask(task_id);
}

TK_RESULT TransportCoreResumeTask(const int32_t task_id) {
  if (!*g_inited.lock()) return -1;
  return g_taskManager->ResumeTask(task_id);
}

int64_t TransportCoreReadData(int32_t task_id, int32_t clip_no, size_t offset,
                              size_t size, char *buf) {
  if (!*g_inited.lock()) return -1;
  return g_taskManager->ReadData(task_id, clip_no, offset, size, buf);
}

void TransportCoreGetProxyURL(const int32_t task_id, char *buf,
                              const size_t buf_size) {
  if (!*g_inited.lock()) return;
  const std::string proxy_url = g_taskManager->GetProxyURL(task_id);
  const size_t size = KFC_MIN(buf_size, proxy_url.size());
  ::memcpy(buf, proxy_url.data(), size);
  buf[size - 1] = '\0';
}
