#include "TransportCore/Task/TaskScheduler.h"

#include "TransportCore/Log/Log.h"

namespace TransportCore {
void TaskScheduler::Start() { run_loop_->AddTimer(this); }
void TaskScheduler::Stop() { run_loop_->RemoveTimer(this); }
void TaskScheduler::OnSchedule() {
  tick_++;
  TC_INFO("");
}
}  // namespace TransportCore
