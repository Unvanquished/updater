#!/usr/bin/env bash

# Any command line arguments will be passed to aria2 configure.

set -e

if [ ! -f src/aria2api.cc ]; then
    echo 'Error: must build from root of aria2 tree'
    exit 1
fi

case "$(uname)" in
        Darwin*)
        export MACOSX_DEPLOYMENT_TARGET=10.13  # To match Qt's target
        export AUTOPOINT=$(brew --prefix gettext)/bin/autopoint
    ;;
esac

num_processors=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

set -x # echo commands

git clean -dXff
autoreconf -i

# For crypto/TLS we use OpenSSL on Linux and the OS-provided one on others.
./configure \
    --enable-static --disable-shared \
    --enable-libaria2 \
    --disable-metalink --disable-websocket --disable-nls \
    --without-libnettle --without-libgmp --without-libgcrypt --without-gnutls \
    --without-sqlite3 --without-libxml2 --without-libexpat --without-libcares --without-libz --without-libssh2 \
    "$@"

# Build only src/ to skip the gettext infrastructure targets which, at some point on Mac, complained about
# gettext being newer than the version required in configure.ac
cd src
MAKEFLAGS="-j${num_processors} ${MAKEFLAGS}" make
