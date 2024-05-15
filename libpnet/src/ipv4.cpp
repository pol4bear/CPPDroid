#include "l3/ipv4.h"
#include <memory.h>
#include <stdexcept>
#include <sstream>
#include <arpa/inet.h>

using namespace std;

namespace pol4b {

IPv4Addr::IPv4Addr() {
  memset(data, 0, sizeof(data));
}

IPv4Addr::IPv4Addr(uint32_t addr) {
  *(uint32_t*)data = addr;
}

IPv4Addr::IPv4Addr(const char *addr) {
  if (addr == nullptr)
    return;
  int addr_len = strlen(addr);
  uint32_t tmp = 0;
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
    tmp <<= 8;
    tmp |= num;
    token_count++;
    token = strtok(nullptr, ".");
  }
  free(tmp_addr);
  if (token_count != 4)
    throw invalid_argument("Invalid IP address.");
  *(uint32_t*)data = tmp;
}

IPv4Addr::operator std::string() const {
  uint32_t *addr = (uint32_t*)data;
  stringstream result;
  result << ((*addr >> 24) & 0xFF) << "." << ((*addr >> 16) & 0xFF) << "." <<
    ((*addr >> 8) & 0xFF) << "." << (*addr & 0xFF);
  return result.str();
}

IPv4Addr::operator uint32_t() const {
  return *(uint32_t*)data;
}

void IPv4Addr::copy(uint8_t *dest, bool network) const {
  if (dest == nullptr)
    return;
  uint32_t value = *this;
  if (network)
    value = htonl(value);
  *(uint32_t*)dest = value;
}

};
