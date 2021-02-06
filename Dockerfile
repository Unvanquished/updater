# See Dockerfile.win for an explanation of some aspects of this file.
FROM debian:buster-slim
# OpenSSL build requires perl
# Qt tarball requires xz-utils
# Qt build requires libgl1-mesa-dev, libxkbcommon-dev, python, zlib1g-dev
# aria2 build requires autoconf, autopoint, gettext
# updater build requires git
RUN apt-get update && apt-get install -y \
    autoconf \
    autopoint \
    curl \
    gettext \
    git \
    g++ \
    libgl1-mesa-dev \
    libtool \
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
RUN curl -LO https://www.openssl.org/source/openssl-1.1.1i.tar.gz && \
    echo $(curl -L https://www.openssl.org/source/openssl-1.1.1i.tar.gz.sha1) openssl-1.1.1i.tar.gz | sha1sum --check
RUN tar -xzf openssl-1.1.1i.tar.gz
WORKDIR /build-ssl/openssl-1.1.1i
RUN ./config no-shared --prefix=/openssl
RUN make -j`nproc` && make install_sw && rm -rf /build-ssl

############
# Build Qt #
############
WORKDIR /build-qt
# We set the results of the renameat2 and statx feature tests to failure with sed below, but the
# statx test is actually ignored so this extra patch is needed to disable it.
COPY disable-statx.patch .
ENV UPDATER_MODULES=qtbase,qtquickcontrols,qtquickcontrols2,qtsvg,qtgraphicaleffects
RUN curl -LO https://download.qt.io/archive/qt/5.14/5.14.2/single/qt-everywhere-src-5.14.2.tar.xz && \
    curl -L https://download.qt.io/archive/qt/5.14/5.14.2/single/md5sums.txt | md5sum --check --ignore-missing && \
    tar -xJf qt-everywhere-src-5.14.2.tar.xz && \
    cd qt-everywhere-src-5.14.2 && \
    sed -i -E 's/tests[.](statx|renameat2)/false/' qtbase/src/corelib/configure.json && \
    patch qtbase/src/corelib/io/qfilesystemengine_unix.cpp < ../disable-statx.patch && \
    OPENSSL_LIBS='-L/openssl/lib -lssl -lcrypto -lpthread -ldl' ./configure -opensource -confirm-license -release -optimize-size -no-shared -static --c++std=14 -nomake tests -nomake tools -nomake examples -no-gif -no-icu -no-glib -no-qml-debug -opengl desktop -no-eglfs -no-opengles3 -no-angle -no-egl -qt-xcb -xkbcommon -dbus-runtime -qt-freetype -qt-pcre -qt-harfbuzz -qt-libpng -qt-libjpeg -system-zlib -I /openssl/include -openssl-linked -prefix /qt && \
    bash -c "make -j`nproc` module-{$UPDATER_MODULES} && make module-{$UPDATER_MODULES}-install_subtargets" && \
    rm -rf /build-qt

###############
# Build aria2 #
###############
COPY aria2 /updater2/aria2
COPY .git/modules/aria2 /updater2/.git/modules/aria2
WORKDIR /updater2/aria2
RUN git clean -dXff
RUN autoreconf -i && PKG_CONFIG_PATH=/openssl/lib/pkgconfig ./configure --without-libxml2 --without-libexpat --without-sqlite3 --enable-libaria2 --without-zlib --without-libcares --enable-static=yes ARIA2_STATIC=yes --without-libssh2 --disable-websocket --disable-nls --with-openssl && make -j`nproc`

#################
# Build updater #
#################
COPY . /updater2
RUN set -e; for D in . quazip fluid; do cd /updater2/$D && git clean -dXff; done
WORKDIR /build
RUN /qt/bin/qmake -config release QMAKE_LFLAGS+="-no-pie" /updater2 && make -j`nproc`
RUN mv updater2 updater2-nonstripped && strip updater2-nonstripped -o updater2
# Version check: do not depend on glibc > 2.26
RUN echo GLIBC_2.26 > target_version && \
    grep -aoE 'GLIBC_[0-9.]+' updater2 > symbol_versions && \
    cat target_version symbol_versions | sort -V | tail -1 > max_version && \
    diff -q target_version max_version
ARG release
RUN if [ -n "$release" ]; then cp updater2 UnvanquishedUpdater && 7z -tzip -mx=9 a UnvUpdaterLinux.zip UnvanquishedUpdater; fi
ENV zipfile=${release:+UnvUpdaterLinux.zip}
CMD cp updater2 updater2-nonstripped $zipfile /build-docker
