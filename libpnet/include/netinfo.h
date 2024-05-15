#pragma once

#include "l2/l2.h"
#include "l3/l3.h"
#include <vector>

namespace pol4b {

class NetInfo {
public:
  NetInfo() {};
  MACAddr mac;
  IPv4Addr ip;
  SubnetMask mask;
};

};
