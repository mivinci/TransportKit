#pragma once
#include "TX/Own.h"
#include "TX/Ref.h"
#include "TX/RunLoop.h"
#include "TX/Thread.h"

namespace TX {
class RunLoopThread {
 public:
  static Own<RunLoopThread> Spawn(const String &name = "RunLoop") {
    auto thread = RunLoop::SpawnThread(name);
    //    thread->Detach();
    const auto run_loop = RunLoop::FromThread(thread->GetId());
    return Own(new RunLoopThread(std::move(thread), run_loop));
  }

  ~RunLoopThread() { runloop_->Stop(); }
  Ref<RunLoop> GetRunLoop() { return runloop_; }

 private:
  explicit RunLoopThread(Own<Thread> thread, const Ref<RunLoop> &run_loop)
      : thread_(std::move(thread)), runloop_(run_loop) {}

 private:
  Own<Thread> thread_{};
  Ref<RunLoop> runloop_;
};
}  // namespace TX
