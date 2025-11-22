#include "TransportCore/task/TaskId.h"

namespace TransportCore {

int32_t TaskId::Next(const TransportCoreTaskKind kind, const int32_t start,
                     const int32_t span) noexcept {
  static std::atomic<int32_t> s_taskId(0);
  const int32_t base = start * (static_cast<int32_t>(kind) + 1);
  const int32_t offset = s_taskId.fetch_add(span, std::memory_order_relaxed);
  return offset + base;
}

TransportCoreTaskKind TaskId::GetKind(const int32_t taskId, const int32_t start) {
  return static_cast<TransportCoreTaskKind>(taskId / start);
}

} // namespace TransportCore
