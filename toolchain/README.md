# 안드로이드 크로스 컴파일

## 프로파일

### 사용법

1. 프로파일 로드

   ```bash
   # 32 bit
   source profile/armv7a.profile
   # 64 bit
   source profile/aarch64.profile
   ```

2. Makefile 생성 & 빌드

   ```bash
   cmake
   make
   ```

### 비고

- profile/default.profile의 Android SDK/NDK 경로 수정 필요

## CMake 툴체인

### 사용법

1. Makefile 생성

   ```bash
   # 32 bit
   cmake -DCMAKE_TOOLCHAIN_FILE=cmake/armv7a.cmake
   # 64 bit
   cmake -DCMAKE_TOOLCHAIN_FILE=cmake/aarch64.cmake
   ```

2. 빌드

   ```bash
   make
   ```

### 비고

- cmake/default.cmake의 Android SDK/NDK 수정 필요