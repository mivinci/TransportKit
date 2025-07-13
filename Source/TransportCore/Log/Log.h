#pragma once

#include "KFC/Log.h"
#include "TransportCore/API/TransportCore.h"

namespace TransportCore {
#define TK_INFO(...) KFC_SCOPE_INFO("TransportCore", __VA_ARGS__)
#define TK_FATAL(...) KFC_SCOPE_FATAL("TransportCore", __VA_ARGS__)

// The design of KFC::Logger by far makes the Logger below weired, we need to
// re-design KFC::Logger.
class Logger final : public KFC::Logger::Reporter, public KFC::Logger::Formatter {
 public:
  explicit Logger(const TransportCoreLogCallback callback,
                  const TransportCoreLogFormat format)
      : callback_(callback), format_(format) {}

  void Init() {
    KFC::Logger::SetReporter(this);
    KFC::Logger::SetFormatter(this);
  }

  void Report(const KFC::Logger::Log &log) override {
    if (callback_) {
      const std::string log_str = Format(log);
      callback_(static_cast<int>(log.m_level), log.m_scope.c_str(), log_str.c_str());
    } else {
      KFC::Logger::Reporter::Report(log);
    }
  }

  std::string Format(const KFC::Logger::Log &log) override {
    switch (format_) {
      case kTransportCoreLogFormatJSON:
        return formatJSON(log);
      default:
        return KFC::Logger::Formatter::Format(log);
    }
  }

 private:
  KFC_NODISCARD static std::string formatJSON(const KFC::Logger::Log &);

 private:
  TransportCoreLogCallback callback_;
  TransportCoreLogFormat format_;
};
}  // namespace TransportCore
