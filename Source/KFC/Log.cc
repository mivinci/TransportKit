/*
* TODO: refactor
 */

#include "KFC/Log.h"

#include <cstddef>
#include <cstdio>

#include "KFC/Exception.h"
#include "KFC/String.h"

#ifdef USE_FMT
#include "fmt/base.h"
#include "fmt/format.h"
#endif // USE_FMT

#ifdef _WIN32
#else
#include <cstdlib>
#endif

#ifdef USE_FMT
template <> struct fmt::formatter<KFC::Logger::Level> : formatter<string_view> {
  format_context::iterator format(KFC::Logger::Level level, format_context &ctx) const {
    string_view name = "U";
    switch (level) {
    case KFC::Logger::Level::Trace:
      name = "T";
      break;
    case KFC::Logger::Level::Debug:
      name = "D";
      break;
    case KFC::Logger::Level::Info:
      name = "I";
      break;
    case KFC::Logger::Level::Warn:
      name = "W";
      break;
    case KFC::Logger::Level::Error:
      name = "E";
      break;
    case KFC::Logger::Level::Fatal:
      name = "F";
      break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};
#else
const char *getLogLevelString(const KFC::Logger::Level level) {
  switch (level) {
  case KFC::Logger::Level::Trace:
    return "T";
  case KFC::Logger::Level::Debug:
    return "D";
  case KFC::Logger::Level::Info:
    return "I";
  case KFC::Logger::Level::Warn:
    return "W";
  case KFC::Logger::Level::Error:
    return "E";
  case KFC::Logger::Level::Fatal:
    return "F";
  default:
    return "";
  }
}
#endif // USE_FMT

namespace KFC {
static Logger::Reporter gReporter;
static Logger::Formatter gFormatter;
Logger::Level Logger::m_level = Level::Debug;
Logger::Reporter *Logger::m_reporter = &gReporter;
Logger::Formatter *Logger::m_formatter = &gFormatter;

void Logger::Log::clean() {
  const auto pos = m_file.find_last_of('/');
  if (pos != String::npos) {
    m_file = m_file.substr(pos + 1);
  } else {
#ifdef TK_SOURCE_DIR
    m_file = m_file.substr(sizeof TK_SOURCE_DIR);
#endif
  }
}

void Logger::Log::output() const {
  m_reporter->Report(*this);
  if (m_level == Level::Fatal) abort();
}

String Logger::Formatter::Format(const Logger::Log &log) {
#ifdef USE_FMT
  return fmt::format("[{:s}][{:s}][{:s}][{:s}:{:d}][{:s}] {}", log.level_, log.time_.Format(),
                     log.scope_, log.file_, log.line_, log.function_, log.message_);
#else
  char buf[128];
  std::snprintf(buf, sizeof(buf), "[%s][%s][%s][%s:%d][%s] %s", getLogLevelString(log.m_level),
                log.m_time.toString().c_str(), log.m_scope.c_str(), log.m_file.c_str(), log.m_line,
                log.m_function.c_str(), log.m_message.c_str());
  return buf;
#endif // USE_FMT
}

void Logger::Reporter::Report(const Log &log) {
#ifdef USE_FMT
  fmt::println("{:s}", formatter_->Format(log));
#else
  printf("%s\n", m_formatter->Format(log).c_str());
#endif // USE_FMT
}
} // namespace KFC
