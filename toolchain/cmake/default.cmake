SET(ANDROID_SDK_ROOT /opt/android-sdk)
SET(ANDROID_NDK_ROOT "${ANDROID_SDK_ROOT}/ndk-bundle")

IF(NOT DEFINED MIN_SDK_VERSION)
  SET(MIN_SDK_VERSION 24)
ENDIF()

SET(ANDROID_PLATFORM "android-${MIN_SDK_VERSION}")
INCLUDE(${ANDROID_NDK_ROOT}/build/cmake/android.toolchain.cmake)

