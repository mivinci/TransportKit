#include "TransportCore/Global/Global.h"

#include "TX/RunLoopThread.h"

namespace TransportCore {
static TX::Own<TX::RunLoopThread> gMainRunLoopThread;

TX::Ref<TX::RunLoop> GetMainRunLoop() {
  if (TX_UNLIKELY(!gMainRunLoopThread)) {
    gMainRunLoopThread = TX::RunLoopThread::Spawn("TKMainRunLoop");
  }
  return gMainRunLoopThread->GetRunLoop();
}
}  // namespace TransportCore
