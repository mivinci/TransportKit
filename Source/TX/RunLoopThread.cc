#include "TX/RunLoopThread.h"

namespace TX {
static Mutex<Own<RunLoopThread>> g_runLoopThread;

Ref<RunLoop> GetGlobalRunLoop() {
  auto guard = g_runLoopThread.Lock();
  if (TX_UNLIKELY(!(*guard))) {
    *guard = RunLoopThread::Spawn("TXGlobalRunLoop");
  }
  return (*guard)->GetRunLoop();
}

}  // namespace TX