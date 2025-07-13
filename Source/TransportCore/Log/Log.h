#pragma once

#include "TX/Log.h"
#include "TransportCore/API/TransportCore.h"

namespace TransportCore {
#define TK_INFO(...) TX_INFO_SCOPE("TransportCore", __VA_ARGS__)
#define TK_FATAL(...) TX_FATAL_SCOPE("TransportCore", __VA_ARGS__)

// The design of TX::Logger by far makes the Logger below weired, we need to
// re-design TX::Logger.
class Logger final : public TX::Logger::Reporter, public TX::Logger::Formatter {
 public:
  explicit Logger(const TransportCoreLogCallback callback,
                  const TransportCoreLogFormat format)
      : callback_(callback), format_(format) {}

  void Init() {
    TX::Logger::SetReporter(this);
    TX::Logger::SetFormatter(this);
  }

  void Report(const TX::Logger::Log &log) override {
    if (callback_) {
      const std::string log_str = Format(log);
      callback_(static_cast<int>(log.m_level), log.m_scope.c_str(), log_str.c_str());
    } else {
      TX::Logger::Reporter::Report(log);
    }
  }

  std::string Format(const TX::Logger::Log &log) override {
    switch (format_) {
      case kTransportCoreLogFormatJSON:
        return formatJSON(log);
      default:
        return TX::Logger::Formatter::Format(log);
    }
  }

 private:
  TX_NODISCARD static std::string formatJSON(const TX::Logger::Log &);

 private:
  TransportCoreLogCallback callback_;
  TransportCoreLogFormat format_;
};
}  // namespace TransportCore
