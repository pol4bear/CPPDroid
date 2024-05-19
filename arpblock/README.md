# ARP Block

## Quick Start

1. 바이너리 빌드

   ```bash
   cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=../toolchain/cmake/aarch64.cmake
   cmake --build build
   ```

2. 실행

   ```bash
   ./arpblock <Target IP>
   ```
