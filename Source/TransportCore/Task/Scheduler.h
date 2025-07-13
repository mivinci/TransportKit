#pragma once

#include <utility>

#include "KFC/Ticker.h"
#include "TransportCore/API/TransportCore.h"

namespace TransportCore {

class Scheduler : public KFC::AtomicRefCounted<Scheduler> {
 public:
  explicit Scheduler(const int32_t task_id,
                     const TransportCoreTaskContext &context)
      : m_ticker(this, &Scheduler::Schedule, KFC::Duration::Second(1),
                 "Scheduler"),
        m_context(context),
        m_taskId(task_id) {}

  virtual void OnStart() = 0;
  virtual void OnStop() = 0;
  virtual void OnPause() = 0;
  virtual void OnResume() = 0;
  virtual void OnSchedule(KFC::Tick) = 0;
  virtual int64_t ReadData(int32_t, size_t, size_t, char *) = 0;
  virtual std::string GetProxyURL() = 0;

  TK_RESULT Start();
  TK_RESULT Stop();
  TK_RESULT Pause();
  TK_RESULT Resume();

  void Schedule(const KFC::Tick tick) {
    // Run the internal schedule callback.
    OnSchedule(tick);
    // See if we need to run the task's custom schedule callback.
    if (m_context.schedule) {
      m_context.schedule(tick, m_context.context);
    }
  }

 private:
  KFC::Ticker<Scheduler> m_ticker;
  TransportCoreTaskContext m_context;
  int32_t m_taskId;
};

}  // namespace TransportCore
