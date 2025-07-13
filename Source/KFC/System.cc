#include "KFC/System.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace KFC {
int getProcessorCoreCount() {
#ifdef _WIN32
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
#else
  return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

} // namespace KFC
