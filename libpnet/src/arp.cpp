#include "l2/l2.h"
#include "netinfomanager.h"
#include <stdexcept>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <netinet/ether.h>
#include <memory.h>

using namespace std;
using namespace pol4b;

MACAddr pol4b::ARP::get_mac_addr(IPv4Addr ip_addr) {
  MACAddr mac_addr;
  int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
  if (sock < 0)
    throw runtime_error("Failed to create socket.");

  auto route_info = NetInfoManager::instance().get_best_routeinfo(ip_addr);
  if (route_info == nullptr)
    throw runtime_error("Failed to get route to IP address.");
  auto if_info = NetInfoManager::instance().get_netinfo(route_info->name);
  if (if_info == nullptr)
    throw runtime_error("Failed to get interface information.");
  int if_index = NetInfoManager::instance().get_interface_index(route_info->name);
  if (if_index == -1)
    throw runtime_error("Failed to get interface index.");
  sockaddr_ll sa;
  memset(&sa, 0, sizeof(sa));
  sa.sll_family = AF_PACKET;
  sa.sll_protocol = htons(ETH_P_ARP);
  sa.sll_ifindex = if_index;
  if (::bind(sock, (sockaddr*)&sa, sizeof(sa)) < 0) {
    close(sock);
    throw runtime_error("Failed to bind socket.");
  }
  struct {
    EthernetHeader eth_hdr;
    ARPHeader arp_hdr;
  } request;
  request.eth_hdr.destination_mac = MACAddr(0xFFFFFFFFFFFF);
  if_info->mac.copy((uint8_t*)&request.eth_hdr.source_mac, true);
  request.arp_hdr.hardware_type = htons((uint16_t)ARPHeader::HardwareType::Ethernet);
  request.arp_hdr.protocol_type = htons((uint16_t)ARPHeader::ProtocolType::IPv4);
  request.arp_hdr.hardware_address_length = sizeof(MACAddr);
  request.arp_hdr.protocol_address_length = sizeof(IPv4Addr);
  request.arp_hdr.operation = htons((uint16_t)ARPHeader::Operation::Request);
  if_info->ip.copy((uint8_t*)&request.arp_hdr.sender_protocol_address, true);
  ip_addr.copy((uint8_t*)&request.arp_hdr.target_protocol_address, true);
  if (sendto(sock, &request, sizeof(request), 0, (sockaddr*)&sa, sizeof(sa)) < 0)
    throw runtime_error("Failed to send ARP request.");

  struct {
    EthernetHeader eth_hdr;
    ARPHeader arp_hdr;
  } reply;
  while (true) {
    auto n = recvfrom(sock, &reply, sizeof(reply), 0, NULL, NULL);
    if (n < 0)
      throw runtime_error("Failed to receive ARP reply.");

    if (reply.eth_hdr.ether_type == htons((uint16_t)EthernetHeader::Ethertype::ARP) &&
      reply.arp_hdr.operation == htons((uint16_t)ARPHeader::Operation::Reply) &&
      request.arp_hdr.sender_protocol_address == reply.arp_hdr.target_protocol_address &&
      request.arp_hdr.target_protocol_address == reply.arp_hdr.sender_protocol_address) {
      mac_addr = reply.arp_hdr.target_hardware_address;
      break;
    }
  }
  close(sock);
  return mac_addr;
}
