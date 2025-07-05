#pragma once
#include "TX/RunLoop.h"
#include "TransportCore/API/TransportCore.h"
#include "TransportCore/Schedule/Scheduler.h"

namespace TransportCore {

class Task {
 public:
  explicit Task(const TX::Ref<TX::RunLoop> &run_loop, const int32_t id,
                const TransportCoreTaskContext &context)
      : id_(id), run_loop_(run_loop), scheduler_(createScheduler(context)) {}

  bool Start() {
    if (!scheduler_) return false;
    return scheduler_->Start();
  }

  bool Stop() {
    if (!scheduler_) return false;
    return scheduler_->Stop();
  }

  bool Pause() {
    if (!scheduler_) return false;
    return scheduler_->Pause();
  }

  bool Resume() {
    if (!scheduler_) return false;
    return scheduler_->Resume();
  }

  int64_t ReadData(int32_t clip_no, size_t offset, size_t size, char *buf) {
    if (!scheduler_) return -1;
    return scheduler_->ReadData(clip_no, offset, size, buf);
  }

  std::string GetProxyURL() {
    if (!scheduler_) return "";
    return scheduler_->GetProxyURL();
  }

  TX_NODISCARD TX::RefPtr<Scheduler> createScheduler(
      const TransportCoreTaskContext &context) const {
    Scheduler *scheduler = nullptr;
    switch (context.kind) {
      case kTransportCoreTaskKindUnSpec:
        scheduler = new Scheduler(run_loop_, id_, context);
        break;
      case kTransportCoreTaskKindPlain:
        scheduler = nullptr;  // TODO
        break;
      default:
        break;
    }
    return TX::adoptRef(scheduler);
  }

 private:
  friend class TaskManager;
  int32_t id_;
  TX::Ref<TX::RunLoop> run_loop_;
  TX::RefPtr<Scheduler> scheduler_;
};
}  // namespace TransportCore
