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
    uint8_t operator[](int index) const;
    void copy(uint8_t *dest, bool network=false) const;
    void to_host_byte_order();

};
#pragma pack(pop)

};
