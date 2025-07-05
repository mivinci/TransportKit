#include "TransportCore/Schedule/Scheduler.h"

#include "TransportCore/Global/Global.h"

namespace TransportCore {
bool Scheduler::Start() {
  run_loop_->AddTimer(this);
  return true;
}
bool Scheduler::Stop() {
  run_loop_->RemoveTimer(this);
  return true;
}

bool Scheduler::Pause() { return false; }

bool Scheduler::Resume() { return false; }

void Scheduler::Schedule() {}

int64_t Scheduler::ReadData(int32_t, size_t, size_t, char *) { return -1; }

std::string Scheduler::GetProxyURL() {
  char buf[256];
  const int64_t size =
      std::snprintf(buf, sizeof(buf), "http://127.0.0.1:%d/proxy/%d",
                    Global::LocalServerPort, task_id_);
  buf[size - 1] = '\0';
  return buf;
}

}  // namespace TransportCore
