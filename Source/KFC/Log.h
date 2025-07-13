/*
 * TODO: refactor
 */

#pragma once
#include <utility>

#include "KFC/Format.h"
#include "KFC/String.h"
#include "KFC/Time.h"

namespace KFC {
#define KFC_LOG(level, scope, ...)                                                                 \
  for (bool shouldLog = KFC::Logger::ShouldLog(KFC::Logger::Level::level); shouldLog;              \
       shouldLog = false)                                                                          \
  KFC::Logger::Log(KFC::Logger::Level::level, __FILE__, __LINE__, __FUNCTION__, scope,             \
                   KFC_FORMAT(__VA_ARGS__))                                                        \
      .output()

#define KFC_TRACE(...) KFC_LOG(Trace, "KFC", __VA_ARGS__)
#define KFC_DEBUG(...) KFC_LOG(Debug, "KFC", __VA_ARGS__)
#define KFC_INFO(...) KFC_LOG(Info, "KFC", __VA_ARGS__)
#define KFC_WARN(...) KFC_LOG(Warn, "KFC", __VA_ARGS__)
#define KFC_ERROR(...) KFC_LOG(Error, "KFC", __VA_ARGS__)
#define KFC_FATAL(...) KFC_LOG(Fatal, "KFC", __VA_ARGS__)

#define KFC_SCOPE_TRACE(scope, ...) KFC_LOG(Trace, scope, __VA_ARGS__)
#define KFC_SCOPE_DEBUG(scope, ...) KFC_LOG(Debug, scope, __VA_ARGS__)
#define KFC_SCOPE_INFO(scope, ...) KFC_LOG(Info, scope, __VA_ARGS__)
#define KFC_SCOPE_WARN(scope, ...) KFC_LOG(Warn, scope, __VA_ARGS__)
#define KFC_SCOPE_ERROR(scope, ...) KFC_LOG(Error, scope, __VA_ARGS__)
#define KFC_SCOPE_FATAL(scope, ...) KFC_LOG(Fatal, scope, __VA_ARGS__)

class Logger {
public:
  enum class Level { Trace, Debug, Info, Warn, Error, Fatal };
  class Log {
  public:
    explicit Log(const Level level, const char *file, const int line, const char *function,
                 const char *scope, String message)
        : m_level(level), m_line(line), m_file(file), m_scope(scope), m_function(function),
          m_message(std::move(message)), m_time(Time::now()) {
      clean();
    }
    explicit Log(const Level level, const char *file, const int line, const char *function,
                 const char *scope)
        : m_level(level), m_line(line), m_file(file), m_scope(scope), m_function(function),
          m_time(Time::now()) {
      clean();
    }

    Log(Log &) = delete;
    void output() const;

  private:
    void clean();

  public:
    Level m_level;
    int m_line;
    String m_file;
    String m_scope;
    String m_function;
    String m_message;
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
    virtual String Format(const Log &);
  };

  static Level m_level;
  static Reporter *m_reporter;
  static Formatter *m_formatter;
  static void SetLevel(const Level level) { m_level = level; }
  static void SetReporter(Reporter *r) { m_reporter = r; }
  static void SetFormatter(Formatter *f) { m_formatter = f; }
  static bool ShouldLog(const Level level) { return level >= m_level; }
};
} // namespace KFC
