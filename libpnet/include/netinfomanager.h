#pragma once

#include "netinfo.h"
#include <vector>

namespace pol4b {

class NetInfoManager {

private:
  NetInfoManager();
  virtual ~NetInfoManager();
  int send_netlink_request(int sock, int type, int flags);

public:
  static NetInfoManager& instance();
  std::vector<NetInfo> get_all_netinfo();
};

};
