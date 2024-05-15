#include "l3/ipv4.h"
#include <memory.h>
#include <stdexcept>
#include <sstream>

using namespace std;

namespace pol4b {

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

};
