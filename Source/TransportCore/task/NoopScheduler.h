#pragma once
#include "TransportCore/task/Scheduler.h"

namespace TransportCore {
class NoopScheduler final : public Scheduler {
 public:
  explicit NoopScheduler(const int32_t task_id,
                         const TransportCoreTaskContext &context)
      : Scheduler(task_id, context) {}
  void OnStart() override {}
  void OnStop() override {}
  void OnPause() override {}
  void OnResume() override {}
  void OnSchedule(KFC::Tick) override {}
  int64_t ReadData(int32_t, size_t, size_t, char *) override { return 0; }
  std::string GetProxyURL() override { return ""; }
};
}  // namespace TransportCore
