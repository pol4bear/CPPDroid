#pragma once

#include <inttypes.h>
#include <string>

namespace pol4b {

#pragma pack(push, 1)
class IPv4Addr {
  public:
    uint8_t data[4];
    IPv4Addr();
    IPv4Addr(uint32_t addr);
    IPv4Addr(const char *addr);
    operator std::string() const;
    operator uint32_t() const;
    void copy(uint8_t *dest, bool network=false) const;
};
#pragma pack(pop)

};
