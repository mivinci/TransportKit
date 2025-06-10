#include "TX/Log.h"

#include "TX/Exception.h"
#include "fmt/base.h"
#include "fmt/format.h"
#ifdef _WIN32
#else
#include <cstdlib>
#endif

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

namespace TX {
static Logger::Reporter GLOBAL_REPORTER;
static Logger::Formatter GLOBAL_FORMATTER;
Logger::Level Logger::level_ = Level::Debug;
Logger::Reporter *Logger::reporter_ = &GLOBAL_REPORTER;
Logger::Formatter *Logger::formatter_ = &GLOBAL_FORMATTER;

void Logger::Log::clean() {
#ifdef TK_SOURCE_DIR
  file_ = file_.substr(sizeof TK_SOURCE_DIR);
#endif
}

void Logger::Log::output() const {
  reporter_->report(*this);
  if (level_ == Level::Fatal) abort();
}

void Logger::Log::throwException() const { throw Exception(message_, scope_); }

std::string Logger::Formatter::format(const Logger::Log &log) {
  return fmt::format("[{:s}][{:s}][{:s}][{:s}:{:d}][{:s}] {}", log.level_,
                     log.time_.Format(), log.scope_, log.file_, log.line_,
                     log.function_, log.message_);
}

void Logger::Reporter::report(const Log &log) {
  fmt::println("{:s}", formatter_->format(log));
}
}  // namespace TX
