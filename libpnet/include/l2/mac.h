#pragma once

#include <inttypes.h>
#include <string>

namespace pol4b {

#pragma pack(push, 1)
class MACAddr {
  public:
    uint8_t data[6];
    MACAddr();
    MACAddr(uint8_t *addr, size_t len);
    MACAddr(uint64_t addr);
    MACAddr(const char *addr);
    operator std::string() const;
    operator uint64_t() const;
    void copy(uint8_t *dest, bool network=false) const;

  private:
    void init(uint64_t addr);
};
#pragma pack(pop)

};
