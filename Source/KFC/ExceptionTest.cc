#include "KFC/RunCatchingExceptions.h"
#include "KFC/Testing.h"

KFC_NAMESPACE_BEG

class MockExceptionCallback final : public Exception::Callback {
public:
  void onFatalException(Exception &&e) override { throw KFC_EXCEPTION(Exception::Logic, "bar"); }
};

TEST(ExceptionTest, RunCatchingExceptions) {
  Option<Exception> e = runCatchingExceptions([] { KFC_THROW_FATAL(Exception::Logic, "foo"); });
  EXPECT_TRUE(e.isSome());
  EXPECT_EQ(e.unwrap().getMessage(), "foo");
}

TEST(ExceptionTest, ExceptionCallback) {
  Option<Exception> e0 = runCatchingExceptions([] { KFC_THROW_FATAL(Exception::Logic, "foo"); });
  EXPECT_TRUE(e0.isSome());
  EXPECT_EQ(e0.unwrap().getMessage(), "foo");

  {
    MockExceptionCallback callback;
    Option<Exception> e1 = runCatchingExceptions([] { KFC_THROW_FATAL(Exception::Logic, "foo"); });
    EXPECT_TRUE(e1.isSome());
    EXPECT_EQ(e1.unwrap().getMessage(), "bar");
  }

  Option<Exception> e2 = runCatchingExceptions([] { KFC_THROW_FATAL(Exception::Logic, "foo"); });
  EXPECT_TRUE(e2.isSome());
  EXPECT_EQ(e2.unwrap().getMessage(), "foo");
}

KFC_NAMESPACE_END