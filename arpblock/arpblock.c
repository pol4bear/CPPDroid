#include <arpa/inet.h>
#include <pcap.h>
#include <if

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage : %s <IP Address>", argv[0]);
    return 1;
  }

  # IP ADDR TRANSLATE

  # ARP HEADER SET

  # WHILE(1) { BROADCAST, SLEEP }

  return 0;
}
