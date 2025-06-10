#include "TransportCore/Task/Task.h"

namespace TransportCore {
void Task::Start() {}
void Task::Stop() {}
void Task::Pause() {}
void Task::Resume() {}
std::string Task::GetProxyURL() const { return ""; }

}  // namespace TransportCore
