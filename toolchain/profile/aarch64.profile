export PROFILE_ROOT=$(dirname $(readlink -f $0))
source $PROFILE_ROOT/default.profile

export TARGET="aarch64-linux-android23"
export CFLAGS="-target $TARGET"
export CXXFLAGS="-target $TARGET"
export LDFLAGS="-target $TARGET"
