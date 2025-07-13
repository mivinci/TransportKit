#include "KFC/StackTrace.h"

KFC_NAMESPACE_BEG

int getStackTrace(void **frames, size_t size, int skipFrames) {
  size = backtrace(frames, size);
  skipFrames++; // skip this function
  memmove(frames, frames + skipFrames, (size - skipFrames) * sizeof(void *));
  return size - skipFrames;
}

String stringifyStackTrace(void **frames, int size) {
  String lines[32];
  char **symbols = backtrace_symbols(frames, size);
  size = std::min(size, 32);
  for (int i = 0; i < size; i++) {
    // TODO: demangle using abi::__cxa_demangle
    lines[i] = symbols[i];
  }
  free(symbols);
  return joinStringArray(lines, "\n");
}

String getStackTraceAsString(const int skipFrames) {
  void *frames[32];
  const int size = getStackTrace(frames, KFC::size(frames), skipFrames);
  return stringifyStackTrace(frames, size);
}

KFC_NAMESPACE_END
