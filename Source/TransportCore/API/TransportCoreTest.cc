#include "TransportCore/API/TransportCore.h"
#include "gtest/gtest.h"

class TransportCoreTest : public ::testing::Test {
 public:
  void SetUp() override { TransportCoreInit(); }
  void TearDown() override { TransportCoreDestroy(); }
};

TEST_F(TransportCoreTest, Simple) {}
