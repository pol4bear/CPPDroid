#include "netinfomanager.h"
#include <iostream>

using namespace std;
using namespace pol4b;

int main() {
  auto interfaces = NetInfoManager::instance().get_netinfo();
  cout << "---------- Network Info -----------" << endl;
  for (auto &interface : interfaces) {
    cout << interface.first << " : " << (string)interface.second.mac << ", " <<
      (string)interface.second.ip << "/" << interface.second.mask.to_cidr() << endl;
  }
  auto routes = NetInfoManager::instance().get_routeinfo();

  cout << endl << "---------- Route Info -----------" << endl;
  for (auto &route : routes) {
    cout << route.name << " : " << (string)route.destination << "/" << route.mask.to_cidr() << ", " <<
      (string)route.gateway << ", " << (uint32_t)route.metric << endl;
  }

  cout << endl << "---------- Best Route to 8.8.8.8 ----------" << endl;
  auto &best_route = NetInfoManager::instance().get_best_routeinfo();
  cout << best_route.name << " : " << (string)best_route.destination << "/" << best_route.mask.to_cidr() << ", " <<
    (string)best_route.gateway << ", " << (uint32_t)best_route.metric << endl;

  return 0;
}
