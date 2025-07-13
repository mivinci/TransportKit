#include "TX/Log.h"

#include <cstddef>
#include <cstdio>
#include <string>

#include "TX/Exception.h"

#ifdef USE_FMT
#include "fmt/base.h"
#include "fmt/format.h"
#endif  // USE_FMT

#ifdef _WIN32
#else
#include <cstdlib>
#endif

#ifdef USE_FMT
template <>
struct fmt::formatter<TX::Logger::Level> : formatter<string_view> {
  format_context::iterator format(TX::Logger::Level level,
                                  format_context &ctx) const {
    string_view name = "U";
    switch (level) {
      case TX::Logger::Level::Trace:
        name = "T";
        break;
      case TX::Logger::Level::Debug:
        name = "D";
        break;
      case TX::Logger::Level::Info:
        name = "I";
        break;
      case TX::Logger::Level::Warn:
        name = "W";
        break;
      case TX::Logger::Level::Error:
        name = "E";
        break;
      case TX::Logger::Level::Fatal:
        name = "F";
        break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};
#else
const char *GetLogLevelString(TX::Logger::Level level) {
  switch (level) {
    case TX::Logger::Level::Trace:
      return "T";
    case TX::Logger::Level::Debug:
      return "D";
    case TX::Logger::Level::Info:
      return "I";
    case TX::Logger::Level::Warn:
      return "W";
    case TX::Logger::Level::Error:
      return "E";
    case TX::Logger::Level::Fatal:
      return "F";
    default:
      return "";
  }
}
#endif  // USE_FMT

namespace TX {
static Logger::Reporter gReporter;
static Logger::Formatter gFormatter;
Logger::Level Logger::m_level = Level::Debug;
Logger::Reporter *Logger::m_reporter = &gReporter;
Logger::Formatter *Logger::m_formatter = &gFormatter;

void Logger::Log::clean() {
  const auto pos = m_file.find_last_of('/');
  if (pos != std::string::npos) {
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

void Logger::Log::throwException() const { throw Exception(m_message, m_scope); }

std::string Logger::Formatter::Format(const Logger::Log &log) {
#ifdef USE_FMT
  return fmt::format("[{:s}][{:s}][{:s}][{:s}:{:d}][{:s}] {}", log.level_,
                     log.time_.Format(), log.scope_, log.file_, log.line_,
                     log.function_, log.message_);
#else
  char buf[128];
  std::snprintf(buf, sizeof(buf), "[%s][%s][%s][%s:%d][%s] %s",
                GetLogLevelString(log.m_level), log.m_time.Format().c_str(),
                log.m_scope.c_str(), log.m_file.c_str(), log.m_line,
                log.m_function.c_str(), log.m_message.c_str());
  return buf;
#endif  // USE_FMT
}

void Logger::Reporter::Report(const Log &log) {
#ifdef USE_FMT
  fmt::println("{:s}", formatter_->Format(log));
#else
  printf("%s\n", m_formatter->Format(log).c_str());
#endif  // USE_FMT
}
}  // namespace TX
