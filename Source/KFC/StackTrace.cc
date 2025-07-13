#include "KFC/StackTrace.h"

KFC_NAMESPACE_BEG

constexpr int kMaxStackTraceDepth = 32;

#ifdef _WIN32
// TODO
#else
int getStackTrace(void **frames, size_t size, int skipFrames) {
  size = backtrace(frames, static_cast<int>(size));
  skipFrames++; // skip this function
  memmove(frames, frames + skipFrames, (size - skipFrames) * sizeof(void *));
  return static_cast<int>(size) - skipFrames;
}

String stringifyStackTrace(void **frames, int size) {
  String lines[kMaxStackTraceDepth];
  char **symbols = backtrace_symbols(frames, size);
  size = std::min(size, kMaxStackTraceDepth);
  for (int i = 0; i < size; i++) {
    lines[i] = demangleStackTraceLine(symbols[i]);
  }
  free(symbols);
  return joinStringArray(lines, "\n");
}

String demangleStackTraceLine(const StringView &line) {
  const size_t start = line.find("_Z");
  const size_t end = line.rfind(" +");
  if (start == String::npos || end == String::npos) {
    // no mangled symbol
    return line.toString();
  }

  String buf;
  buf.resize(line.size() * 2);
  // copy up to the mangled symbol
  memcpy(buf.data(), line.data(), start);

  int status;
  const String sym(line.data() + start, end - start);
  char *demangledSym = abi::__cxa_demangle(sym.c_str(), nullptr, nullptr, &status);
  if (status != 0) {
    // demangling failed
    return line.toString();
  }
  // copy the demangled symbol
  const size_t demangledSize = strlen(demangledSym);
  memcpy(buf.data() + start, demangledSym, demangledSize);
  free(demangledSym);

  // copy the rest
  const size_t restStart = start + demangledSize;
  const size_t restSize = line.size() - end;
  memcpy(buf.data() + restStart, line.data() + end, restSize);
  buf.resize(restStart + restSize);
  return buf;
}

#endif

String getStackTraceAsString(const int skipFrames) {
  void *frames[kMaxStackTraceDepth];
  const int size = getStackTrace(frames, KFC::size(frames), skipFrames);
  return stringifyStackTrace(frames, size);
}

KFC_NAMESPACE_END
