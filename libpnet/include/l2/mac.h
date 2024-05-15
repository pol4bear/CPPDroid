#pragma once

#include <inttypes.h>
#include <string>

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

};
