#pragma once

#ifdef _WIN32
#include <Windows.h>
#define TX_SLEEP(sec) Sleep(sec * 1000);
#else
#include <unistd.h>
#define TX_SLEEP(sec) sleep(sec)
#endif

namespace TX {}
