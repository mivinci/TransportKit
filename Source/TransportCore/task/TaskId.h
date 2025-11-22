#pragma once

#include "TransportCore/API/TransportCore.h"

#include <atomic>

namespace TransportCore {

constexpr int32_t kTaskIdBase = 100000;
constexpr int8_t kTaskIdSpan = 1;

class TaskId {
public:
  static int32_t Next(const TransportCoreTaskKind kind, const int32_t start,
                      const int32_t span) noexcept;
  static TransportCoreTaskKind GetKind(const int32_t taskId, const int32_t start);
};
} // namespace TransportCore
