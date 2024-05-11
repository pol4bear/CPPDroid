#pragma once

#include <inttypes.h>
#include <cstring>
#include <string>
#include <vector>

namespace pol4b {

class MACAddr {
  public:
    uint8_t data[6];
    MACAddr();
    MACAddr(uint8_t *addr, size_t len);
    MACAddr(uint64_t addr);
    MACAddr(const char *addr);
    operator std::string() const;
    operator uint64_t() const;

  private:
    void init(uint64_t addr);
};

class IPv4Addr {
  public:
    uint8_t data[4];
    IPv4Addr();
    IPv4Addr(uint32_t addr);
    IPv4Addr(const char *addr);
    operator std::string() const;
    operator uint32_t() const;
};

class SubnetMask : public IPv4Addr {
  public:
    SubnetMask();
    SubnetMask(uint32_t addr);
    SubnetMask(const char *addr);
    static SubnetMask from_cidr(int cidr);
    int to_cidr() const;
};

class NetInfo {
public:
  NetInfo();
  std::string name;
  MACAddr mac;
  IPv4Addr ip;
  SubnetMask mask;
  IPv4Addr gateway;
};

class NetInfoManager {

private:
  NetInfoManager();
  virtual ~NetInfoManager();
  int send_netlink_request(int sock, int type, int flags);

public:
  static NetInfoManager& instance();
  std::vector< NetInfo> get_all_netinfo();
};

};
