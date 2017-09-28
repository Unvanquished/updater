FROM dolcetriade/qt5-linux-static-openssl:5.9.0 
RUN apt-get update && apt-get install -y autopoint autoconf gettext libcppunit-dev libtool libgcrypt11-dev pkgconf git
COPY . /updater2
WORKDIR /updater2/aria2
RUN autoreconf -i && ./configure --without-libxml2 --without-libexpat --without-sqlite3 --enable-libaria2 --without-zlib --without-libcares --enable-static=yes ARIA2_STATIC=yes --without-libssh2 --disable-websocket && make clean && make -j`nproc`
WORKDIR /updater2
RUN qmake -config release && make clean && make -j`nproc`
CMD cp updater2 /build-docker
