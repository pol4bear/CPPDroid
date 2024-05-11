#include "pnetinfo.h"
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <sys/types.h>
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

#define FREE_IF_NOT_NULL(p) if(p) free(p)

namespace pol4b {

void MACAddr::init(uint64_t addr) {
  this->data[0] = (addr >> 40) & 0xFF;
  this->data[1] = (addr >> 32) & 0xFF;
  this->data[2] = (addr >> 24) & 0xFF;
  this->data[3] = (addr >> 16) & 0xFF;
  this->data[4] = (addr >> 8) & 0xFF;
  this->data[5] = addr & 0xFF;
}

MACAddr::MACAddr() {
  memset(this->data, 0, sizeof(this->data));
}

MACAddr::MACAddr(uint8_t * addr, size_t len) {
  if (len != 6)
    throw invalid_argument("Invalid MAC address");
  this->data[0] = addr[0];
  this->data[1] = addr[1];
  this->data[2] = addr[2];
  this->data[3] = addr[3];
  this->data[4] = addr[4];
  this->data[5] = addr[5];
}

MACAddr::MACAddr(uint64_t addr) {
  init(addr);
}

MACAddr::MACAddr(const char *addr) {
  string tmp_addr = addr;
  tmp_addr.erase(remove(tmp_addr.begin(), tmp_addr.end(), '-'), tmp_addr.end());
  tmp_addr.erase(remove(tmp_addr.begin(), tmp_addr.end(), ':'), tmp_addr.end());
  if (tmp_addr.size() != 12)
    throw invalid_argument("Invalid MAC address.");

  size_t pos;
  uint64_t converted = stoul(tmp_addr, &pos, 16);
  if (pos != 12)
    throw invalid_argument("Invalid MAC address.");
  init(converted);
}

MACAddr::operator std::string() const {
  stringstream result;
  for (int i = 0; i < 5; i++)
    result << hex << uppercase << setw(2) << setfill('0') << static_cast<unsigned>(this->data[i]) << ":";
  result << hex << uppercase << setw(2) << setfill('0') << static_cast<unsigned>(this->data[5]);
  return result.str();
}

MACAddr::operator uint64_t() const {
  uint64_t result = (uint64_t)this->data[0] << 40 | (uint64_t)this->data[1] << 32 | (uint64_t)this->data[2] << 24 |
    (uint64_t)this->data[3] << 16 | (uint64_t)this->data[4] << 8 | (uint64_t)this->data[5];
  return result;
}

IPv4Addr::IPv4Addr() {
  memset(this->data, 0, sizeof(this->data));
}

IPv4Addr::IPv4Addr(uint32_t addr) {
  this->data[0] = (addr >> 24) & 0xFF;
  this->data[1] = (addr >> 16) & 0xFF;
  this->data[2] = (addr >> 8) & 0xFF;
  this->data[3] = addr & 0xFF;
}

IPv4Addr::IPv4Addr(const char *addr) {
  if (addr == nullptr)
    return;
  int addr_len = strlen(addr);
  if (addr_len < 7 || addr_len > 15)
    throw invalid_argument("Address length must be between 7 and 15 characters.");
  char *tmp_addr = strdup(addr);
  if (tmp_addr == nullptr)
    throw bad_alloc();
  char *token = strtok(tmp_addr, ".");
  int token_count = 0;
  while (token != nullptr && token_count < 4) {
    int num = atoi(token);
    if (num < 0 || num > 255)
      throw invalid_argument("Address must be between 0.0.0.0 and 255.255.255.255.");
    this->data[token_count++] = num;
    token = strtok(nullptr, ".");
  }
  free(tmp_addr);
  if (token_count != 4)
    throw invalid_argument("Invalid IP address.");
}

IPv4Addr::operator std::string() const {
  stringstream result;
  result << (int)this->data[0] << "." << (int)this->data[1] << "." <<
    (int)this->data[2] << "." << (int)this->data[3];
  return result.str();
}

IPv4Addr::operator uint32_t() const {
  uint32_t result = this->data[0] << 24 | this->data[1] << 16 | this->data[2] << 8 | this->data[3];
  return result;
}

SubnetMask::SubnetMask() : IPv4Addr() {}

SubnetMask::SubnetMask(const char *addr) : IPv4Addr(addr) {}


SubnetMask::SubnetMask(uint32_t addr) : IPv4Addr(addr) {}

SubnetMask SubnetMask::from_cidr(int cidr) {
  return SubnetMask(0xFFFFFFFF << (32 - cidr));
}

int SubnetMask::to_cidr() const {
  uint32_t data = (uint32_t)*this;
  if (data == 0)
    return 0;
  int zero_count = 0;
  for (; (data & 1) == 0; zero_count++)
    data >>= 1;
  return 32 - zero_count;
}

NetInfo::NetInfo() {}

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
