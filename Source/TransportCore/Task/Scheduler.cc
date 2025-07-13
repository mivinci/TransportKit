#include "TransportCore/Task/Scheduler.h"

#include "TransportCore/Log/Log.h"

namespace TransportCore {
TK_RESULT Scheduler::Start() {
  if (!m_ticker.Start()) return TK_ERR;
  m_ticker.Perform(&Scheduler::OnStart);
  TK_INFO("task: %d(%s), start", m_taskId, m_context.keyid);
  return TK_OK;
}

TK_RESULT Scheduler::Stop() {
  if (!m_ticker.Stop()) return TK_ERR;
  m_ticker.Perform(&Scheduler::OnStop);
  TK_INFO("task: %d(%s), stop", m_taskId, m_context.keyid);
  return TK_OK;
}

TK_RESULT Scheduler::Pause() {
  if (!m_ticker.Stop()) return TK_ERR;
  m_ticker.Perform(&Scheduler::OnPause);
  TK_INFO("task: %d(%s), pause", m_taskId, m_context.keyid);
  return TK_OK;
}

TK_RESULT Scheduler::Resume() {
  if (!m_ticker.Start()) return TK_ERR;
  m_ticker.Perform(&Scheduler::OnResume);
  TK_INFO("task: %d(%s), resume", m_taskId, m_context.keyid);
  return TK_OK;
}
}  // namespace TransportCore
