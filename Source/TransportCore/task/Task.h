#pragma once
#include "TransportCore/API/TransportCore.h"
#include "TransportCore/task/NoopScheduler.h"

namespace TransportCore {
// We can actually implement Start/Stop/Pause/Resume/ReadData etc. directly in
// class `Task` but we don't for two reasons.
// 1. A task is more like a container holding meta-information about a specific
// work while a scheduler sounds more like how the work is done. Semantics
// always matters.
// 2. We can do some argument checks in `Task`'s methods before calling into
// schedulers. Otherwise, every scheduler will have to do those checks that are
// so bloated and unnecessary.
class Task {
 public:
  explicit Task(const int32_t id, const TransportCoreTaskContext &context)
      : m_id(id), m_scheduler(createScheduler(context)) {}

  TK_RESULT Start() {
    if (!m_scheduler) return TK_ERR;
    return m_scheduler->Start();
  }

  TK_RESULT Stop() {
    if (!m_scheduler) return TK_ERR;
    return m_scheduler->Stop();
  }

  TK_RESULT Pause() {
    if (!m_scheduler) return TK_ERR;
    return m_scheduler->Pause();
  }

  TK_RESULT Resume() {
    if (!m_scheduler) return TK_ERR;
    return m_scheduler->Resume();
  }

  int64_t ReadData(const int32_t clip_no, const size_t offset,
                   const size_t size, char *buf) {
    if (!m_scheduler) return TK_ERR;
    return m_scheduler->ReadData(clip_no, offset, size, buf);
  }

  std::string GetProxyURL() {
    if (!m_scheduler) return "";
    return m_scheduler->GetProxyURL();
  }

  KFC_NODISCARD KFC::RefPtr<Scheduler> createScheduler(
      const TransportCoreTaskContext &context) const {
    Scheduler *scheduler = nullptr;
    switch (context.kind) {
      case kTransportCoreTaskKindUnSpec:
        scheduler = new NoopScheduler(m_id, context);
        break;
      case kTransportCoreTaskKindPlain:
        scheduler = nullptr;  // TODO
        break;
      default:
        break;
    }
    return KFC::adoptRef(scheduler);
  }

 private:
  friend class TaskManager;
  int32_t m_id;
  KFC::RefPtr<Scheduler> m_scheduler;
};
}  // namespace TransportCore
