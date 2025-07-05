#include "TransportCore/API/TransportCore.h"

#include "TX/RunLoopThread.h"
#include "TransportCore/Global/Global.h"
#include "TransportCore/Log/Log.h"
#include "TransportCore/Schedule/TaskManager.h"

struct GlobalContext {
  TransportCore::TaskManager *task_manager = nullptr;
  TX::Own<TX::RunLoopThread> main_run_loop_thread;
  bool initialized = false;
};

static TX::Mutex<GlobalContext> globalContextGuard;

void TransportCoreInit() {
  auto global_context = globalContextGuard.Lock();
  if (global_context->initialized)
    TK_FATAL("TransportCore already initialized");
  global_context->main_run_loop_thread =
      TX::RunLoopThread::Spawn("TransportCoreMainRunLoop");
  global_context->task_manager = new TransportCore::TaskManager(
      global_context->main_run_loop_thread->GetRunLoop());
  global_context->initialized = true;
  global_context->task_manager->Start();
}

void TransportCoreDestroy() {
  auto global_context = globalContextGuard.Lock();
  if (!global_context->initialized) TK_FATAL("TransportCore not initialized");
  global_context->task_manager->Stop();
  delete global_context->task_manager;
  auto _ = std::move(global_context->main_run_loop_thread);
  global_context->initialized = false;
}

int32_t TransportCoreCreateTask(const TransportCoreTaskContext &context) {
  auto global_context = globalContextGuard.Lock();
  if (!global_context->initialized) TK_FATAL("TransportCore not initialized");
  return global_context->task_manager->CreateTask(context);
}

void TransportCoreStartTask(const int32_t task_id) {
  auto global_context = globalContextGuard.Lock();
  if (!global_context->initialized) TK_FATAL("TransportCore not initialized");
  global_context->task_manager->StartTask(task_id);
}

void TransportCoreStopTask(const int32_t task_id) {
  auto global_context = globalContextGuard.Lock();
  if (!global_context->initialized) TK_FATAL("TransportCore not initialized");
  global_context->task_manager->StopTask(task_id);
}

void TransportCorePauseTask(const int32_t task_id) {
  auto global_context = globalContextGuard.Lock();
  if (!global_context->initialized) TK_FATAL("TransportCore not initialized");
  global_context->task_manager->PauseTask(task_id);
}

void TransportCoreResumeTask(const int32_t task_id) {
  auto global_context = globalContextGuard.Lock();
  if (!global_context->initialized) TK_FATAL("TransportCore not initialized");
  global_context->task_manager->ResumeTask(task_id);
}

int64_t TransportCoreReadData(int32_t task_id, int32_t clip_no, size_t offset,
                              size_t size, char *buf) {
  auto global_context = globalContextGuard.Lock();
  if (!global_context->initialized) TK_FATAL("TransportCore not initialized");
  return global_context->task_manager->ReadData(task_id, clip_no, offset, size,
                                                buf);
}

void TransportCoreGetProxyURL(const int32_t task_id, char *buf,
                              const size_t buf_size) {
  auto global_context = globalContextGuard.Lock();
  if (!global_context->initialized) TK_FATAL("TransportCore not initialized");
  const std::string proxy_url =
      global_context->task_manager->GetProxyURL(task_id);
  const size_t size = std::min(buf_size, proxy_url.size());
  std::memcpy(buf, proxy_url.data(), size);
  buf[size - 1] = '\0';
}
