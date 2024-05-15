#pragma once

#include "netinfo.h"
#include "routeinfo.h"
#include <vector>
#include <map>
#include <unordered_map>

namespace pol4b {

class NetInfoManager {

private:
  NetInfoManager();
  virtual ~NetInfoManager();

  std::map<int, std::string> interface_name;
  std::unordered_map<std::string, NetInfo> interfaces;
  std::vector<RouteInfo> routes;
  int send_netlink_request(int sock, int type, int flags);


public:
  static NetInfoManager& instance();
  void load_netinfo();
  void load_routeinfo();
  const std::unordered_map<std::string, NetInfo> &get_netinfo(bool reload=false);
  const std::vector<RouteInfo> &get_routeinfo(bool reload=false);
  const RouteInfo &get_best_routeinfo(IPv4Addr destination="8.8.8.8");
};

};
