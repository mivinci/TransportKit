#pragma once

#include "KFC/Preclude.h"

#ifdef __GNUC__
#define KFC_THREAD_LOCAL __thread
#else
#define KFC_THREAD_LOCAL thread_local
#endif

KFC_NAMESPACE_BEG
KFC_NAMESPACE_END
