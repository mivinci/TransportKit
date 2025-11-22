#include "TransportCore/task/Scheduler.h"

namespace TransportCore {
TK_RESULT Scheduler::Start() {
  if (!m_scheduleHandle.start()) return TK_ERR;
  m_scheduleHandle.queueWork(&Scheduler::OnStart);
  return TK_OK;
}

TK_RESULT Scheduler::Stop() {
  if (!m_scheduleHandle.stop()) return TK_ERR;
  m_scheduleHandle.queueWork(&Scheduler::OnStop);
  return TK_OK;
}

TK_RESULT Scheduler::Pause() {
  if (!m_scheduleHandle.stop()) return TK_ERR;
  m_scheduleHandle.queueWork(&Scheduler::OnPause);
  return TK_OK;
}

TK_RESULT Scheduler::Resume() {
  if (!m_scheduleHandle.start()) return TK_ERR;
  m_scheduleHandle.queueWork(&Scheduler::OnResume);
  return TK_OK;
}
} // namespace TransportCore
