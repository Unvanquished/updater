# See Dockerfile.win for an explanation of some aspects of this file.
FROM docker.io/debian:bullseye-slim
# OpenSSL build requires perl
# Qt tarball requires xz-utils
# Qt build requires libgl1-mesa-dev, libxkbcommon-dev, python, zlib1g-dev
# Qt configuration additionally finds libxcb-glx0-dev/libx11-xcb-dev/libxext-dev. Without them, the
# updater builds but icons (gear, download, etc.) are mysteriously missing, when built in the
# Bullseye environment. It may be that not all are necessary.
# aria2 build requires autoconf, autopoint, gettext
# git is used for cleaning unwanted files
RUN apt-get update && apt-get install -y \
    autoconf \
    autopoint \
    curl \
    gettext \
    git \
    g++ \
    libgl1-mesa-dev \
    libtool \
    libx11-xcb-dev \
    libxcb-glx0-dev \
    libxext-dev \
    libxkbcommon-dev \
    make \
    perl \
    p7zip-full \
    pkg-config \
    python \
    xz-utils \
    zlib1g-dev

#################
# Build OpenSSL #
#################
WORKDIR /build-ssl
RUN curl -LO https://github.com/openssl/openssl/releases/download/openssl-3.6.0/openssl-3.6.0.tar.gz && \
    curl -L https://github.com/openssl/openssl/releases/download/openssl-3.6.0/openssl-3.6.0.tar.gz.sha256 | sha256sum --check
RUN tar -xzf openssl-3.6.0.tar.gz
WORKDIR /build-ssl/openssl-3.6.0
RUN ./config --prefix=/openssl --openssldir=/dev/null no-shared no-apps no-autoload-config no-capieng no-dso no-dynamic-engine no-engine no-loadereng no-module
RUN make -j`nproc` && make install_sw && rm -rf /build-ssl

############
# Build Qt #
############
WORKDIR /build-qt
ENV UPDATER_MODULES=qtbase,qtquickcontrols,qtquickcontrols2,qtsvg,qtgraphicaleffects
RUN curl -LO https://download.qt.io/archive/qt/5.14/5.14.2/single/qt-everywhere-src-5.14.2.tar.xz && \
    curl -L https://download.qt.io/archive/qt/5.14/5.14.2/single/md5sums.txt | md5sum --check --ignore-missing && \
    tar -xJf qt-everywhere-src-5.14.2.tar.xz && \
    cd qt-everywhere-src-5.14.2 && \
    OPENSSL_LIBS='-L/openssl/lib64 -lssl -lcrypto -lpthread -ldl' ./configure -opensource -confirm-license -release -optimize-size -no-shared -static --c++std=14 -nomake tests -nomake tools -nomake examples -no-gif -no-icu -no-glib -no-qml-debug -opengl desktop -no-eglfs -no-opengles3 -no-angle -no-egl -qt-xcb -xkbcommon -dbus-runtime -qt-freetype -qt-pcre -qt-harfbuzz -qt-libpng -qt-libjpeg -system-zlib -I /openssl/include -openssl-linked -prefix /qt && \
    bash -c "make -j`nproc` module-{$UPDATER_MODULES} && make module-{$UPDATER_MODULES}-install_subtargets" && \
    rm -rf /build-qt

###############
# Build aria2 #
###############
COPY aria2 /updater/aria2
COPY .git/modules/aria2 /updater/.git/modules/aria2
COPY build-aria.sh /updater/
WORKDIR /updater/aria2
RUN OPENSSL_LIBS='-L/openssl/lib64 -lssl -lcrypto -lpthread -ldl' OPENSSL_CFLAGS='-I /openssl/include' ../build-aria.sh --with-openssl

#################
# Build updater #
#################
COPY . /updater
RUN set -e; for D in . quazip fluid; do cd /updater/$D && git clean -dXff; done
WORKDIR /build
RUN /qt/bin/qmake -config release QMAKE_LFLAGS+="-no-pie" /updater && make -j`nproc`
RUN mv updater updater-nonstripped && strip updater-nonstripped -o updater
# Version check: do not depend on glibc > 2.31
RUN echo GLIBC_2.31 > target_version && \
    grep -aoE 'GLIBC_[0-9.]+' updater > symbol_versions && \
    cat target_version symbol_versions | sort -V | tail -1 | tee max_version && \
    diff -q target_version max_version
ARG release
RUN if [ -n "$release" ]; then cp updater UnvanquishedUpdater && 7z -tzip -mx=9 a UnvUpdaterLinux.zip UnvanquishedUpdater; fi
ENV zipfile=${release:+UnvUpdaterLinux.zip}
CMD cp updater updater-nonstripped $zipfile /build-docker
