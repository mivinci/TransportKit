#pragma once
#include "TX/Log.h"
namespace TransportCore {
#define TK_INFO(...) TX_INFO_SCOPE("TransportCore", __VA_ARGS__)
#define TK_FATAL(...) TX_FATAL_SCOPE("TransportCore", __VA_ARGS__)
}
