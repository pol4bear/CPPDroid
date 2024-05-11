#include "pnetinfo.h"
#include <iostream>

using namespace std;
using namespace pol4b;

int main() {
  vector<NetInfo> interfaces = NetInfoManager::instance().get_all_netinfo();
  for (const auto &interface : interfaces) {
    cout << interface.name << "\t: " << (string)interface.mac << ", " <<
      (string)interface.ip << "/" << interface.mask.to_cidr() << ", " <<
      (string)interface.gateway << endl;
  }
  return 0;
}
