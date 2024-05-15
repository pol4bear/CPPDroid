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

vector<NetInfo> NetInfoManager::get_all_netinfo() {
  int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  if (sock < 0)
    throw runtime_error("Failed to create netlink socket.");
  int flags = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);
  sockaddr_nl addr;
  memset(&addr, 0 , sizeof(addr));
  addr.nl_family = AF_NETLINK;

  if (::bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
    close(sock);
    throw runtime_error("Failed to bind netlink socket.");
  }

  char buffer[8192];
  int len = 0;
  map<int, NetInfo> interfaces;
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
              interfaces[iface->ifi_index].name = (char*)RTA_DATA(attr);
            else if (attr->rta_type == IFLA_ADDRESS)
              interfaces[iface->ifi_index].mac = MACAddr((uint8_t*)RTA_DATA(attr), 6);
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
          memcpy(interfaces[ifa->ifa_index].ip.data, RTA_DATA(attr), sizeof(uint32_t));
          break;
        }
      }
      interfaces[ifa->ifa_index].mask = SubnetMask::from_cidr((int)ifa->ifa_prefixlen);
    }
  }
  }
  send_netlink_request(sock, RTM_GETROUTE, NLM_F_DUMP);
  while ((len = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
  for (nlmsghdr *nh = (nlmsghdr*)buffer; NLMSG_OK(nh, len); nh = NLMSG_NEXT(nh, len)) {
    if (nh->nlmsg_type == NLMSG_DONE)
      break;
    else if (nh->nlmsg_type == RTM_NEWROUTE) {
      rtmsg *rtm = (rtmsg *)NLMSG_DATA(nh);
      rtattr *attr = (rtattr *)RTM_RTA(rtm);
      int length = RTM_PAYLOAD(nh);
      int interface_index = -1;
      uint32_t gateway = 0;
      for (; RTA_OK(attr, length); attr = RTA_NEXT(attr, length)) {
          if (attr->rta_type == RTA_GATEWAY)
            memcpy(&gateway, RTA_DATA(attr), sizeof(gateway));
          else if (attr->rta_type == RTA_OIF)
            memcpy(&interface_index, RTA_DATA(attr), sizeof(interface_index));
      }
      if (interface_index != -1 && gateway != 0)
        interfaces[interface_index].gateway = IPv4Addr(ntohl(gateway));
    }
  }
  }

  vector<NetInfo> result;
  for (const auto &interface : interfaces)
    result.push_back(interface.second);

  return result;
}

};
