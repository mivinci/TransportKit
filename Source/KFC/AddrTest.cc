#include "KFC/Addr.h"
#include "KFC/Testing.h"

namespace KFC {
TEST(AddrTest, ParseIPv4) {
  Result<IPv4Addr, AddrParseError> addr = KFC_OK(AddrParseError::Internal);
  EXPECT_EQ(addr.unwrapErr(), AddrParseError::Internal);

  addr = IPv4Addr::parse("0.0.0.0");
  EXPECT_TRUE(addr.unwrap().isUnspecified());
  addr = IPv4Addr::parse("127.0.0.1");
  EXPECT_TRUE(addr.unwrap().isLoopback());
  addr = IPv4Addr::parse("255.255.255.255");
  EXPECT_TRUE(addr.unwrap().isBroadcast());
  addr = IPv4Addr::parse("10.0.0.1");
  EXPECT_TRUE(addr.unwrap().isPrivate());
  addr = IPv4Addr::parse("172.16.0.1");
  EXPECT_TRUE(addr.unwrap().isPrivate());
  addr = IPv4Addr::parse("192.168.0.1");
  EXPECT_TRUE(addr.unwrap().isPrivate());
  addr = IPv4Addr::parse("169.254.1.1");
  EXPECT_TRUE(addr.unwrap().isLinkLocal());
  addr = IPv4Addr::parse("224.0.0.1");
  EXPECT_TRUE(addr.unwrap().isMulticast());
  addr = IPv4Addr::parse("240.0.0.1");
  EXPECT_TRUE(addr.unwrap().isReserved());

  addr = IPv4Addr::parse("256.0.0.1");
  EXPECT_EQ(addr.unwrapErr(), AddrParseError::InvalidIPv4);
  addr = IPv4Addr::parse("1.2.3");
  EXPECT_EQ(addr.unwrapErr(), AddrParseError::InvalidIPv4);
  addr = IPv4Addr::parse("1.2.3.4.5");
  EXPECT_EQ(addr.unwrapErr(), AddrParseError::InvalidIPv4);
  addr = IPv4Addr::parse("::1");
  EXPECT_EQ(addr.unwrapErr(), AddrParseError::InvalidIPv4);
}

TEST(AddrTest, ParseIPv6) {
  Result<IPv6Addr, AddrParseError> addr = KFC_OK(AddrParseError::Internal);
  EXPECT_EQ(addr.unwrapErr(), AddrParseError::Internal);

  addr = IPv6Addr::parse("::");
  EXPECT_TRUE(addr.unwrap().isUnspecified());
  addr = IPv6Addr::parse("::1");
  EXPECT_TRUE(addr.unwrap().isLoopback());
  addr = IPv6Addr::parse("fe80::1");
  EXPECT_TRUE(addr.unwrap().isLinkLocal());
  addr = IPv6Addr::parse("ff00::1");
  EXPECT_TRUE(addr.unwrap().isMulticast());

  addr = IPv6Addr::parse("::g");
  EXPECT_EQ(addr.unwrapErr(), AddrParseError::InvalidIPv6);
  addr = IPv6Addr::parse("::1::1");
  EXPECT_EQ(addr.unwrapErr(), AddrParseError::InvalidIPv6);
}

TEST(AddrTest, IPv4ToIPv6) {
  const IPv4Addr addr = IPv4Addr::parse("127.0.0.1").unwrap();
  EXPECT_EQ(addr.toIPv6Compatible(), IPv6Addr::parse("::127.0.0.1").unwrap());
  EXPECT_EQ(addr.toIPv6Mapped(), IPv6Addr::parse("::ffff:127.0.0.1").unwrap());
}

TEST(AddrTest, ToString) {
  EXPECT_EQ(IPv4Addr::parse("127.0.0.1").unwrap().toString(), "127.0.0.1");
  EXPECT_EQ(IPv6Addr::parse("::1").unwrap().toString(), "::1");
}

TEST(AddrTest, IPAddr) {
  const auto a1 = IPAddr::parse("10.0.0.1");
  const auto a2 = IPAddr::parse("::1");
  EXPECT_EQ(a1.unwrap().toString(), "10.0.0.1");
  EXPECT_EQ(a2.unwrap().toString(), "::1");
}

TEST(AddrTest, SocketAddr) {
  const auto a1 = SocketAddr::parse("127.0.0.1:80");
  const auto a2 = SocketAddr::parse("[::1]:80");
  EXPECT_EQ(a1.unwrap().toString(), "127.0.0.1:80");
  EXPECT_EQ(a2.unwrap().toString(), "[::1]:80");
}
} // namespace KFC
