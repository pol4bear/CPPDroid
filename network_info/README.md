# 안드로이드 네트워크 인터페이스 정보

## Quick Start

1. 바이너리 빌드

   ```bash
   cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain/cmake/aarch64.cmake -S . -B build
   cmake --build build
   ```

2. 실행

   ```bash
   angler:/data/local/tmp # ./get_all_netinfo
   ---------- Network Info -----------
   lo : 00:00:00:00:00:00, 127.0.0.1/8
   dummy0 : 76:04:F3:1B:A6:84, 0.0.0.0/0
   sit0 : 00:00:00:00:08:00, 0.0.0.0/0
   wlan0 : 24:DF:6A:63:47:40, 192.168.50.16/24
   
   ---------- Route Info -----------
   dummy0 : 0.0.0.0/0, 0.0.0.0, 0
   wlan0 : 0.0.0.0/0, 192.168.50.1, 0
   wlan0 : 192.168.50.0/0, 0.0.0.0, 0
   wlan0 : 192.168.50.0/28, 0.0.0.0, 0
   lo : 127.0.0.0/32, 0.0.0.0, 0
   lo : 127.0.0.0/32, 0.0.0.0, 0
   lo : 127.0.0.1/32, 0.0.0.0, 0
   lo : 127.255.255.255/32, 0.0.0.0, 0
   wlan0 : 192.168.50.0/28, 0.0.0.0, 0
   wlan0 : 192.168.50.16/28, 0.0.0.0, 0
   wlan0 : 192.168.50.255/28, 0.0.0.0, 0
   
   ---------- Best Route to 8.8.8.8 ----------
   wlan0 : 0.0.0.0/0, 192.168.50.1, 0
   ```

# 참고

- 안드로이드 크로스 컴파일 관련 : https://github.com/pol4bear/CPPDroid/tree/main/toolchain
