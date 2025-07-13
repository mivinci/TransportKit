#pragma once
#include <string>
#include <utility>

#include "TX/Format.h"
#include "TX/Time.h"

namespace TX {
#define TX_LOG(level, scope, ...)                                             \
  for (bool shouldLog = TX::Logger::ShouldLog(TX::Logger::Level::level);      \
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
        : m_level(level),
          m_line(line),
          m_file(file),
          m_scope(scope),
          m_function(function),
          m_message(std::move(message)),
          m_time(Time::Now()) {
      clean();
    }
    explicit Log(const Level level, const char *file, const int line,
                 const char *function, const char *scope)
        : m_level(level),
          m_line(line),
          m_file(file),
          m_scope(scope),
          m_function(function),
          m_time(Time::Now()) {
      clean();
    }

    Log(Log &) = delete;
    void output() const;
    void throwException() const;

   private:
    void clean();

   public:
    Level m_level;
    int m_line;
    std::string m_file;
    std::string m_scope;
    std::string m_function;
    std::string m_message;
    Time m_time;
  };

  class Reporter {
   public:
    virtual ~Reporter() = default;
    virtual void Report(const Log &);
  };

  class Formatter {
   public:
    virtual ~Formatter() = default;
    virtual std::string Format(const Log &);
  };

  static Level m_level;
  static Reporter *m_reporter;
  static Formatter *m_formatter;
  static void SetLevel(const Level level) { m_level = level; }
  static void SetReporter(Reporter *r) { m_reporter = r; }
  static void SetFormatter(Formatter *f) { m_formatter = f; }
  static bool ShouldLog(const Level level) {
    return level >= m_level;
  }
};
}  // namespace TX
