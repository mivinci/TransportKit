#pragma once

#include "KFC/Preclude.h"
#include "KFC/String.h"

#if _WIN32
#include <Windows.h>
#else
#include <execinfo.h>
#include <cxxabi.h>
#endif

KFC_NAMESPACE_BEG

KFC_NOINLINE int getStackTrace(void **frames, size_t size, int skipFrames = 0);
String stringifyStackTrace(void **frames, int size);
String getStackTraceAsString(int skipFrames = 0);
String demangleStackTraceLine(const StringView &line);

KFC_NAMESPACE_END
