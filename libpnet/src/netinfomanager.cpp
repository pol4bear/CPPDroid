#include "netinfomanager.h"
#include <stdexcept>
#include <memory.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <map>
#include <fcntl.h>
#include <iostream>
#include <cstdint>

using namespace std;

namespace pol4b {

NetInfoManager::NetInfoManager() {}
NetInfoManager::~NetInfoManager() {}

int NetInfoManager::send_netlink_request(int sock, int type, int flags) {
  struct {
      nlmsghdr nlh;
      rtgenmsg genmsg;
  } request;

  memset(&request, 0, sizeof(request));
  request.nlh.nlmsg_len = sizeof(request);
  request.nlh.nlmsg_type = type;
  request.nlh.nlmsg_flags = NLM_F_REQUEST | flags;
  request.nlh.nlmsg_seq = 1;
  request.genmsg.rtgen_family = AF_INET;

  return send(sock, &request, sizeof(request), 0);
}

NetInfoManager &NetInfoManager::instance() {
  static NetInfoManager net_info_manager;
  return net_info_manager;
}

void NetInfoManager::load_netinfo() {
  int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  if (sock < 0)
    throw runtime_error("Failed to create netlink socket.");
  int flags = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);
  sockaddr_nl addr;
  memset(&addr, 0 , sizeof(addr));
  addr.nl_family = AF_NETLINK;

  char buffer[8192];
  int len = 0;
  map<int, NetInfo> interface_map;
  send_netlink_request(sock, RTM_GETLINK, NLM_F_DUMP);
  while ((len = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
    for (nlmsghdr *nh = (nlmsghdr*)buffer; NLMSG_OK(nh, len); nh = NLMSG_NEXT(nh, len)) {
      if (nh->nlmsg_type == NLMSG_DONE)
        break;
      else if (nh->nlmsg_type == RTM_NEWLINK) {
        ifinfomsg *iface = (ifinfomsg *)NLMSG_DATA(nh);
        rtattr *attr = (rtattr *) IFLA_RTA(iface);
        int length = nh->nlmsg_len - NLMSG_LENGTH(sizeof(*iface));
        for (; RTA_OK(attr, length); attr = RTA_NEXT(attr, length)) {
            if (attr->rta_type == IFLA_IFNAME)
              interface_name[iface->ifi_index] = (char*)RTA_DATA(attr);
            else if (attr->rta_type == IFLA_ADDRESS)
              interface_map[iface->ifi_index].mac = MACAddr((uint8_t*)RTA_DATA(attr), 6);
        }
      }
    }
  }
  send_netlink_request(sock, RTM_GETADDR, NLM_F_DUMP);
  while ((len = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
    for (nlmsghdr *nh = (nlmsghdr*)buffer; NLMSG_OK(nh, len); nh = NLMSG_NEXT(nh, len)) {
      if (nh->nlmsg_type == NLMSG_DONE)
        break;
      else if (nh->nlmsg_type == RTM_NEWADDR) {
        ifaddrmsg *ifa = (ifaddrmsg*)NLMSG_DATA(nh);
        rtattr *attr = (rtattr*)IFA_RTA(ifa);
        int ifa_len = IFA_PAYLOAD(nh);
        for (; RTA_OK(attr, ifa_len); attr = RTA_NEXT(attr, ifa_len)) {
          if (attr->rta_type == IFA_LOCAL) {
            memcpy(interface_map[ifa->ifa_index].ip.data, RTA_DATA(attr), sizeof(uint32_t));
            break;
          }
        }
        interface_map[ifa->ifa_index].mask = SubnetMask::from_cidr((int)ifa->ifa_prefixlen);
      }
    }
  }

  interfaces.clear();
  for (const auto &interface : interface_map)
    interfaces[interface_name[interface.first]] = interface.second;
}

void NetInfoManager::load_routeinfo() {
  if (interface_name.size() < 1)
    load_netinfo();

  int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  if (sock < 0)
    throw runtime_error("Failed to create netlink socket.");
  int flags = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);
  sockaddr_nl addr;
  memset(&addr, 0 , sizeof(addr));
  addr.nl_family = AF_NETLINK;

  char buffer[8192];
  int len = 0;
  send_netlink_request(sock, RTM_GETROUTE, NLM_F_DUMP);
  while ((len = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
    for (nlmsghdr *nh = (nlmsghdr*)buffer; NLMSG_OK(nh, len); nh = NLMSG_NEXT(nh, len)) {
      if (nh->nlmsg_type == NLMSG_DONE)
        break;
      else if (nh->nlmsg_type == RTM_NEWROUTE) {
        rtmsg *rtm = (rtmsg *)NLMSG_DATA(nh);
        rtattr *attr = (rtattr *)RTM_RTA(rtm);
        int length = RTM_PAYLOAD(nh);
        uint32_t tmp = 0;
        RouteInfo route_info;
        for (; RTA_OK(attr, length); attr = RTA_NEXT(attr, length)) {
          switch(attr->rta_type) {
          case RTA_GATEWAY:
            memcpy(&tmp, RTA_DATA(attr), sizeof(tmp));
            route_info.gateway = IPv4Addr(ntohl(tmp));
            break;
          case RTA_DST:
            memcpy(&tmp, RTA_DATA(attr), sizeof(tmp));
            route_info.destination = IPv4Addr(ntohl(tmp));
            break;
          case RTA_PREFSRC:
            memcpy(&tmp, RTA_DATA(attr), sizeof(tmp));
            route_info.mask = SubnetMask(ntohl(tmp));
            break;
          case RTA_PRIORITY:
            memcpy(&route_info.metric, RTA_DATA(attr), sizeof(route_info.metric));
            break;
          case RTA_OIF:
            memcpy(&tmp, RTA_DATA(attr), sizeof(tmp));
            route_info.name = interface_name[tmp];
            break;
          }
        }
        routes.push_back(route_info);
      }
    }
  }
}

const unordered_map<string, NetInfo> &NetInfoManager::get_netinfo(bool reload) {
  if (reload || interfaces.size() < 1)
    load_netinfo();
  return interfaces;
}

const vector<RouteInfo> &NetInfoManager::get_routeinfo(bool reload) {
  if (reload || routes.size() < 1)
    load_routeinfo();
  return routes;
}

const RouteInfo &NetInfoManager::get_best_routeinfo(IPv4Addr destination) {
  const RouteInfo *best_route = nullptr;
  int longest_prefix = -1;

  for (auto &route : routes) {
    if ((destination & route.mask) == route.destination) {
      int prefix_len = __builtin_popcount(route.mask);
      if (prefix_len > longest_prefix || (prefix_len == longest_prefix && route.metric < best_route->metric)) {
        longest_prefix = prefix_len;
        best_route = &route;
      }
    }
  }

  return *best_route;
}

};
