#pragma once

#include "KFC/RunLoop.h"

namespace KFC {

class WaitScope {
 public:
  explicit WaitScope(RunLoop &runLoop) : m_runLoop(runLoop) { m_runLoop.enterScope(); }
  ~WaitScope() { m_runLoop.leaveScope(); }
  KFC_DISALLOW_COPY_AND_MOVE(WaitScope)

 private:
  RunLoop &m_runLoop;
};

}  // namespace KFC
