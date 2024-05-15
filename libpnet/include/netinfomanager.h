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
  std::unordered_map<std::string, int> interface_index;
  std::unordered_map<std::string, NetInfo> interfaces;
  std::vector<RouteInfo> routes;
  int send_netlink_request(int sock, int type, int flags);


public:
  static NetInfoManager& instance();
  void load_netinfo();
  void load_routeinfo();
  const std::unordered_map<std::string, NetInfo> &get_all_netinfo(bool reload=false);
  const std::vector<RouteInfo> &get_all_routeinfo(bool reload=false);
  const NetInfo *get_netinfo(std::string name);
  const RouteInfo *get_best_routeinfo(IPv4Addr destination="8.8.8.8");
  std::string get_interface_name(int index);
  int get_interface_index(std::string name);
};

};
