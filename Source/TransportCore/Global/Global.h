#pragma once

#include "TX/RunLoop.h"

namespace TransportCore {

// A module should run in the main run loop obtained by `GetMainRunLoop` if
// it is not compute-intensive.
TX::Ref<TX::RunLoop> GetMainRunLoop();

// If a module does need to run in its own run loop, call `GetWorkerRunLoop` to
// get one.
TX::Ref<TX::RunLoop> GetWorkerRunLoop();

}  // namespace TransportCore
