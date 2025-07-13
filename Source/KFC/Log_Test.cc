#include "KFC/Exception.h"
#include "KFC/Log.h"
#include "KFC/Testing.h"

namespace KFC {
class MockReporter final : public Logger::Reporter {
public:
  MockReporter() : n(0) {}
  void Report(const Logger::Log &) override { n++; }
  int n;
};

TEST(LogTest, Simple) {
  KFC_DEBUG("The ultimate answer to the universe is %d", 42);
  EXPECT_ANY_THROW(KFC_THROW(KFC::Exception::Kind::Logic, "This is an exception."));
}

TEST(LogTest, Reporter) {
  const auto r = new MockReporter;
  Logger::SetReporter(r);
  Logger::SetLevel(Logger::Level::Error);
  KFC_DEBUG("debug");
  KFC_INFO("info");
  KFC_WARN("warn");
  KFC_ERROR("error");
  EXPECT_EQ(r->n, 1);
}
} // namespace KFC
