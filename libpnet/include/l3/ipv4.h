#pragma once

#include <inttypes.h>
#include <string>

namespace pol4b {

class IPv4Addr {
  public:
    uint8_t data[4];
    IPv4Addr();
    IPv4Addr(uint32_t addr);
    IPv4Addr(const char *addr);
    operator std::string() const;
    operator uint32_t() const;
};

};
