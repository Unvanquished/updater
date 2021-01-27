#!/bin/bash
set -e
cd aria2
git clean -dXff
export MACOSX_DEPLOYMENT_TARGET=10.13  # To match Qt's target
AUTOPOINT=$(brew --prefix gettext)/bin/autopoint autoreconf -i
./configure --without-libxml2 --without-libexpat --without-sqlite3 --enable-libaria2 --without-zlib --without-libcares --without-libgmp --without-libnettle --enable-static=yes ARIA2_STATIC=yes --without-libssh2 --disable-websocket --disable-nls
# Build only src/ to skip the gettext infrastructure targets which complain about
# gettext being newer than the version required in configure.ac
cd src
make -j2
