#pragma once

#include "mac.h"
#include "l3/ipv4.h"

namespace pol4b {

#pragma pack(push, 1)
class ARPHeader {
public:
  ARPHeader() = default;

  uint16_t hardware_type;
  uint16_t protocol_type;
  uint8_t hardware_address_length;
  uint8_t protocol_address_length;
  uint16_t operation;
  MACAddr sender_hardware_address;
  IPv4Addr sender_protocol_address;
  MACAddr target_hardware_address;
  IPv4Addr target_protocol_address;

  enum class HardwareType : uint16_t {
    Ethernet = 1
  };

  enum class ProtocolType : uint16_t {
    IPv4 = 0x0800
  };

  enum class Operation : uint16_t {
    Request = 1,
    Reply = 2
  };
 };
 #pragma pack(pop)

class ARP {
public:
  static MACAddr get_mac_addr(IPv4Addr ip_addr);
};

}