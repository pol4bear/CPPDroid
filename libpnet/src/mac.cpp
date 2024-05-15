#include "l2/mac.h"
#include <algorithm>
#include <stdexcept>
#include <memory.h>
#include <iomanip>

using namespace std;

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


};
