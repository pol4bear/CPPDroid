# 안드로이드 네트워크 인터페이스 리스트

## PCAP 빌드

1. Makefile 생성

   ```bash
   cd libpcap
   # 프로파일 로드
   source ./aarch64.profile
   # configure 스크립트 생성
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

## 네트워크 인터페이스 리스트 출력 바이너리 빌드

1. Makefile 생성

   ```bash
   # 정적 라이브러리 사용
   cmake -DCMAKE_TOOLCHAIN_FILE=aarch64.cmake
   
   # 공유 라이브러리 사용
   cmake -DCMAKE_TOOLCHAIN_FILE=aarch64.cmake -DUSE_SHARED=1
   ```

2. 바이너리 빌드

   ```bash
   make
   ```

3. 실행

   ```bash
   angler:/data/local/tmp # ./interface_list
   1 : any
   2 : lo
   3 : nflog
   4 : nfqueue
   ```

# 참고

- 안드로이드 크로스 컴파일 관련 : https://github.com/pol4bear/CPPDroid/tree/main/toolchain
