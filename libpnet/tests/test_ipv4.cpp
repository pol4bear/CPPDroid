#include "l3/ipv4.h"
#include <gtest/gtest.h>

using namespace std;
using namespace pol4b;

TEST(IPv4Test, BasicAssertions) {
  ASSERT_THROW(IPv4Addr("1"), invalid_argument);
  ASSERT_THROW(IPv4Addr("1111111111111111"), invalid_argument);
  ASSERT_THROW(IPv4Addr("19216801"), invalid_argument);
  ASSERT_THROW(IPv4Addr("192.168.0"), invalid_argument);
  ASSERT_EQ((uint32_t)IPv4Addr("192.168.0.1"), 0xC0A80001);
  ASSERT_EQ((string)IPv4Addr(0xC0A80001), "192.168.0.1");
}