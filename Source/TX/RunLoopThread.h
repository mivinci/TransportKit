#pragma once
#include "TX/Own.h"
#include "TX/Ref.h"
#include "TX/RunLoop.h"
#include "TX/Thread.h"

namespace TX {
class RunLoopThread {
 public:
  static Own<RunLoopThread> Spawn(const std::string &name = "RunLoop") {
    auto thread = RunLoop::SpawnThread(name);
    // If we do not detach here, ~RunLoopThread will block on join,
    // before it stops the underlying run loop.
    thread->Detach();
    const auto run_loop = RunLoop::FromThread(thread->GetId());
    return Own<RunLoopThread>(new RunLoopThread(std::move(thread), run_loop));
  }

  ~RunLoopThread() { m_runloop->Stop(); }
  Ref<RunLoop> GetRunLoop() { return m_runloop; }

 private:
  explicit RunLoopThread(Own<Thread> thread, const Ref<RunLoop> &run_loop)
      : m_thread(std::move(thread)), m_runloop(run_loop) {}

 private:
  Own<Thread> m_thread;
  Ref<RunLoop> m_runloop;
};
}  // namespace TX
