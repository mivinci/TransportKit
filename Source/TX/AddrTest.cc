#include "Addr.h"
#include "gtest/gtest.h"

namespace TX {
TEST(AddrTest, ParseIPv4Addr) {
  auto addr = IPv4Addr::Parse("0.0.0.0");
  EXPECT_TRUE(addr.IsOk());
  EXPECT_EQ(addr.Unwrap().ToBits(), 0x00000000);

  addr = IPv4Addr::Parse("127.0.0.1");
  EXPECT_TRUE(addr.IsOk());
  EXPECT_EQ(addr.Unwrap().ToBits(), 0x7F000001);
  EXPECT_TRUE(addr.Unwrap().IsLoopBack());

  addr = IPv4Addr::Parse("192.168.1.1");
  EXPECT_TRUE(addr.IsOk());
  EXPECT_EQ(addr.Unwrap().ToBits(), 0xC0A80101);
  EXPECT_TRUE(addr.Unwrap().IsPrivate());

  addr = IPv4Addr::Parse("");
  EXPECT_TRUE(addr.IsErr());
  EXPECT_EQ(addr.UnwrapErr(), AddrParseError());

  addr = IPv4Addr::Parse("1.1.1");
  EXPECT_TRUE(addr.IsErr());
  EXPECT_EQ(addr.UnwrapErr(), AddrParseError());

  addr = IPv4Addr::Parse("256.0.0.1");
  EXPECT_TRUE(addr.IsErr());
  EXPECT_EQ(addr.UnwrapErr(), AddrParseError());
}
} // namespace TX
