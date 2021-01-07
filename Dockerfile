# See Dockerfile.win for an explanation of some aspects of this file.

FROM dolcetriade/qt5-linux-static-openssl:5.9.0 
RUN apt-get update && apt-get install -y autopoint autoconf gettext libcppunit-dev libtool libgcrypt11-dev pkgconf git

###############
# Build aria2 #
###############
COPY aria2 /updater2/aria2
COPY .git/modules/aria2 /updater2/.git/modules/aria2
WORKDIR /updater2/aria2
RUN git clean -dXff
RUN autoreconf -i && ./configure --without-libxml2 --without-libexpat --without-sqlite3 --enable-libaria2 --without-zlib --without-libcares --enable-static=yes ARIA2_STATIC=yes --without-libssh2 --disable-websocket --disable-nls && make -j`nproc`

#################
# Build updater #
#################
COPY . /updater2
RUN set -e; for D in . quazip fluid; do cd /updater2/$D && git clean -dXff; done
WORKDIR /build
RUN qmake -config release /updater2 && make -j`nproc`
CMD cp updater2 /build-docker
