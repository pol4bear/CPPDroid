#include "l2/mac.h"
#include <gtest/gtest.h>

using namespace std;
using namespace pol4b;

TEST(MACTest, BasicAssertions) {
  ASSERT_THROW(MACAddr("1"), invalid_argument);
  ASSERT_THROW(MACAddr("1111111111111111"), invalid_argument);
  ASSERT_THROW(MACAddr("ABCDEFGHIJKL"), invalid_argument);
  ASSERT_THROW(MACAddr("AB:CD:EF:GH:IJ:KL"), invalid_argument);
  ASSERT_THROW(MACAddr("AB-CD-EF-GH-IJ-KL"), invalid_argument);
  ASSERT_EQ((uint64_t)MACAddr("AA:BB:CC:DD:EE:FF"), 0xAABBCCDDEEFF);
  ASSERT_EQ((uint64_t)MACAddr("AA-BB-CC-DD-EE-FF"), 0xAABBCCDDEEFF);
  ASSERT_EQ((string)MACAddr(0xAABBCCDDEEFF), "AA:BB:CC:DD:EE:FF");
}
