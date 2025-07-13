#include "TX/Time.h"

namespace TX {
Duration Duration::FOREVER = Duration(INT64_MAX);
bool Time::isZoneSet = false;
}
