# 안드로이드 네트워크 인터페이스 정보

## PCAP 빌드

1. Makefile 생성

   ```bash
   # 프로파일 로드
   source ../toolchain/profile/aarch64.profile
   # configure 스크립트 생성(없을시)
   cd libpcap
   autoconf
   # --prefix : 빌드 결과물 경로
   # --host : 라이브러리를 실행할 호스트
   # --with-pcap : 패킷 캡처 형태
   # --enable-shared : 공유 라이브러리(SO) 빌드 활성화
   ./configure --prefix=$PWD/.. --host=arm-linux --with-pcap=linux --enable-shared
   ```

2. 라이브러리 빌드

   ```bash
   make && make install
   ```

3. 결과물

   ```bash
   # file libpcap.so.1.10.4	
   libpcap.so.1.10.4: ELF 64-bit LSB shared object, ARM aarch64, version 1 (SYSV), dynamically linked, not stripped
   ```

## GoogleTest 빌드

```bash
cd googletest
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=../toolchain/cmake/a
arch64.cmake -DCMAKE_INSTALL_PREFIX=..
cmake --build build
cmake --install build
```

## 네트워크 인터페이스 정보 출력 바이너리 빌드

1. Makefile 생성

   ```bash
   # 정적 라이브러리 사용
   cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain/cmake/aarch64.cmake -S . -B build

   # 공유 라이브러리 사용
   cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain/cmake/aarch64.cmake -DUSE_SHARED=1 -S . -B build
   ```

2. 바이너리 빌드

   ```bash
   cmake --build build
   ```

3. 실행

   ```bash
   angler:/data/local/tmp $ ./get_all_netinfo
   lo      : 00:00:00:00:00:00, 127.0.0.1/0, 0.0.0.0
   dummy0  : E2:4B:E4:E9:A3:93, 0.0.0.0/0, 0.0.0.0
   sit0    : 00:00:00:00:08:00, 0.0.0.0/0, 0.0.0.0
   rmnet_ipa0      : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   wlan0   : 24:DF:6A:63:47:40, 192.168.50.16/0, 192.168.50.1
   rmnet_data0     : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   rmnet_data1     : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   rmnet_data2     : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   rmnet_data3     : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   rmnet_data4     : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   rmnet_data5     : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   rmnet_data6     : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   rmnet_data7     : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   r_rmnet_data0   : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   r_rmnet_data1   : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   r_rmnet_data2   : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   r_rmnet_data3   : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   r_rmnet_data4   : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   r_rmnet_data5   : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   r_rmnet_data6   : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   r_rmnet_data7   : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   r_rmnet_data8   : 00:00:00:00:00:00, 0.0.0.0/0, 0.0.0.0
   ```

# 참고

- 안드로이드 크로스 컴파일 관련 : https://github.com/pol4bear/CPPDroid/tree/main/toolchain
