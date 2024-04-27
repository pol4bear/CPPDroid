export ANDROID_SDK_ROOT=/opt/android-sdk
export ANDROID_NDK_ROOT=$ANDROID_SDK_ROOT/ndk-bundle

export PATH="$ANDROID_SDK_ROOT/cmdline-tools/latest/bin:$ANDROID_SDK_ROOT/ndk-bundle/toolchains/llvm/prebuilt/linux-x86_64/bin:$PATH"
export CC=clang
export CXX=clang++

