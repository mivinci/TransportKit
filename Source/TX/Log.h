#pragma once
#include <string>
#include <utility>

#include "TX/Format.h"
#include "TX/Time.h"

namespace TX {
#define TX_LOG(level, scope, ...)                                             \
  for (bool shouldLog = TX::Logger::shouldLog(TX::Logger::Level::level);      \
       shouldLog; shouldLog = false)                                          \
  TX::Logger::Log(TX::Logger::Level::level, __FILE__, __LINE__, __FUNCTION__, \
                  scope, TX_FORMAT(__VA_ARGS__))                              \
      .output()

#define TX_THROW(...)                                                      \
  for (TX::Logger::Log log(TX::Logger::Level::Error, __FILE__, __LINE__,   \
                           __FUNCTION__, "throw", TX_FORMAT(__VA_ARGS__)); \
       ; log.throwException())

#define TX_TRACE(...) TX_LOG(Trace, "TX", __VA_ARGS__)
#define TX_DEBUG(...) TX_LOG(Debug, "TX", __VA_ARGS__)
#define TX_INFO(...) TX_LOG(Info, "TX", __VA_ARGS__)
#define TX_WARN(...) TX_LOG(Warn, "TX", __VA_ARGS__)
#define TX_ERROR(...) TX_LOG(Error, "TX", __VA_ARGS__)
#define TX_FATAL(...) TX_LOG(Fatal, "TX", __VA_ARGS__)

#define TX_TRACE_SCOPE(scope, ...) TX_LOG(Trace, scope, __VA_ARGS__)
#define TX_DEBUG_SCOPE(scope, ...) TX_LOG(Debug, scope, __VA_ARGS__)
#define TX_INFO_SCOPE(scope, ...) TX_LOG(Info, scope, __VA_ARGS__)
#define TX_WARN_SCOPE(scope, ...) TX_LOG(Warn, scope, __VA_ARGS__)
#define TX_ERROR_SCOPE(scope, ...) TX_LOG(Error, scope, __VA_ARGS__)
#define TX_FATAL_SCOPE(scope, ...) TX_LOG(Fatal, scope, __VA_ARGS__)

class Logger {
 public:
  enum class Level { Trace, Debug, Info, Warn, Error, Fatal };
  class Log {
   public:
    explicit Log(const Level level, const char *file, const int line,
                 const char *function, const char *scope, std::string message)
        : level_(level),
          line_(line),
          file_(file),
          scope_(scope),
          function_(function),
          message_(std::move(message)),
          time_(Time::Now()) {
      clean();
    }
    explicit Log(const Level level, const char *file, const int line,
                 const char *function, const char *scope)
        : level_(level),
          line_(line),
          file_(file),
          scope_(scope),
          function_(function),
          time_(Time::Now()) {
      clean();
    }

    Log(Log &) = delete;
    void output() const;
    void throwException() const;

   private:
    void clean();

   public:
    Level level_;
    int line_;
    std::string_view file_;
    std::string_view scope_;
    std::string_view function_;
    std::string message_;
    Time time_;
  };

  class Reporter {
   public:
    virtual void report(const Log &);
  };

  class Formatter {
   public:
    virtual std::string format(const Log &);
  };

  static Level level_;
  static Reporter *reporter_;
  static Formatter *formatter_;
  static void setLevel(const Level level) { level_ = level; }
  static void setReporter(Reporter *r) { reporter_ = r; }
  static void setFormatter(Formatter *f) { formatter_ = f; }
  static bool shouldLog(const Level level) {
    return level >= level_ || level == Level::Trace;
  }
};
}  // namespace TX
