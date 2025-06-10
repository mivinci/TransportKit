#pragma once
#include "TX/Log.h"
namespace TransportCore {
static auto scope = "TransportCore";
#define TC_INFO(...) TX_INFO_SCOPE(scope, __VA_ARGS__)
}

