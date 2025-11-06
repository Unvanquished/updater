# See Dockerfile.win for an explanation of some aspects of this file.
FROM docker.io/debian:bullseye-slim

# Prevents warning spam from Qt 6's rcc
ENV LANG=C.UTF-8

# OpenSSL build requires perl
# Qt tarball requires xz-utils
# Qt build requires libgl1-mesa-dev, libxkbcommon-dev, python, zlib1g-dev................
# aria2 build requires autoconf, autopoint, gettext
# git is used for cleaning unwanted files

# For Qt's xcb_syslibs compile test
ENV XCB_MINIMUM_PACKAGES=' \
    libxcb-cursor-dev \
    libxcb-icccm4-dev \
    libxcb-image0-dev \
    libxcb-keysyms1-dev \
    libxcb-randr0-dev \
    libxcb-render0-dev \
    libxcb-render-util0-dev \
    libxcb-shape0-dev \
    libxcb-shm0-dev \
    libxcb-sync-dev \
    libxcb-xfixes0-dev \
    libxcb-xkb-dev \
    libxcb-util-dev \
'
RUN echo 'deb http://archive.debian.org/debian bullseye-backports main' > /etc/apt/sources.list.d/backports.list
RUN apt-get update && apt-get install -y \
    autoconf \
    autopoint \
    cmake/bullseye-backports \
    cmake-data/bullseye-backports \
    curl \
    gettext \
    git \
    g++ \
    libgl1-mesa-dev \
    libtool \
    libx11-xcb-dev \
    libxcb-glx0-dev \
    libxkbcommon-x11-dev \
    make \
    ninja-build \
    perl \
    p7zip-full \
    pkg-config \
    python \
    xz-utils \
    zlib1g-dev \
    $XCB_MINIMUM_PACKAGES
RUN rm /usr/lib/x86_64-linux-gnu/libxcb-*.so

#################
# Build OpenSSL #
#################
WORKDIR /build-ssl
COPY sha256sums-openssl.txt /build-ssl/
RUN curl -LO http://github.com/openssl/openssl/releases/download/openssl-3.6.0/openssl-3.6.0.tar.gz && \
    sha256sum --check sha256sums-openssl.txt
RUN tar -xzf openssl-3.6.0.tar.gz
WORKDIR /build-ssl/openssl-3.6.0
RUN ./config --prefix=/openssl --openssldir=/dev/null no-shared no-apps no-autoload-config no-capieng no-dso no-dynamic-engine no-engine no-loadereng no-module -Os no-pic -fno-pic
RUN make -j`nproc` && make install_sw && rm -rf /build-ssl

############
# Build Qt #
############
WORKDIR /build-qt
COPY md5sums-qt.txt build-qt.sh /build-qt/
COPY patches /build-qt/patches
ARG release
# Note: {foo:+bar} here is a syntax of the Dockerfile, not the shell!
ENV IPO_ARG=${release:+-DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON}
RUN BUILDQT_CMAKE_ARGS="${IPO_ARG} -DCMAKE_POSITION_INDEPENDENT_CODE=OFF -DFEATURE_reduce_relocations=OFF" CXXFLAGS='-fno-pic -no-pie' CFLAGS='-fno-pic -no-pie' PKG_CONFIG_PATH=/openssl/lib64/pkgconfig ./build-qt.sh linux && mv qt_linux /qt && rm -rf /build-qt

###############
# Build aria2 #
###############
COPY aria2 /updater/aria2
COPY .git/modules/aria2 /updater/.git/modules/aria2
COPY build-aria.sh /updater/
WORKDIR /updater/aria2
RUN OPENSSL_LIBS='-L/openssl/lib64 -lssl -lcrypto -lpthread -ldl' OPENSSL_CFLAGS='-I /openssl/include' \
    CFLAGS='-Os -fno-pic -no-pie' CXXFLAGS='-Os -fno-pic -no-pie' ../build-aria.sh --with-openssl

#################
# Build updater #
#################
COPY . /updater
RUN set -e; for D in . quazip fluid; do cd /updater/$D && git clean -dXff; done
WORKDIR /build
RUN PKG_CONFIG_PATH=/openssl/lib64/pkgconfig CXXFLAGS='-no-pie -fno-pic' CFLAGS='-no-pie -fno-pic' cmake -G Ninja -DCMAKE_POSITION_INDEPENDENT_CODE=OFF -DCMAKE_FIND_ROOT_PATH=/qt -DCMAKE_BUILD_TYPE=MinSizeRel ${IPO_ARG} /updater && ninja
RUN mv updater updater-nonstripped && strip updater-nonstripped -o updater
# Version check: do not depend on glibc > 2.31
RUN echo GLIBC_2.31 > target_version && \
    grep -aoE 'GLIBC_[0-9.]+' updater > symbol_versions && \
    cat target_version symbol_versions | sort -V | tail -1 | tee max_version && \
    diff -q target_version max_version
RUN if [ -n "$release" ]; then cp updater UnvanquishedUpdater && 7z -tzip -mx=9 a UnvUpdaterLinux.zip UnvanquishedUpdater; fi
ENV zipfile=${release:+UnvUpdaterLinux.zip}
CMD cp updater updater-nonstripped $zipfile /build-docker
