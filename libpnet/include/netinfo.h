#pragma once

#include "l2/l2.h"
#include "l3/l3.h"

namespace pol4b {

class NetInfo {
public:
  NetInfo() {};
  std::string name;
  MACAddr mac;
  IPv4Addr ip;
  SubnetMask mask;
  IPv4Addr gateway;
};

};
