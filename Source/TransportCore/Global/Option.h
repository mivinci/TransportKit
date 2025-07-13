#pragma once

#include "TX/Bits.h"

namespace TransportCore::GlobalOption {
// Using a class with just static members to implement a global object is too
// boilerplate, I'd rather do it with a namespace. Anyway, there is no
// difference once they're compiled to machine code.
static TX::uint16 LocalServerPort = 0;

// Although simple and clean, there's still a chance to do the global stuff in
// other ways in the future, so we better use a macro `GLOBAL_OPTION` to hide
// out the implementation details.
#define GLOBAL_OPTION(K) GlobalOption::K
}  // namespace TransportCore::GlobalOption
