FROM freeslave/qt5-base-static:5.8.0-1
RUN apt-get install -y autoconf gettext libcppunit-dev libtool libgcrypt11-dev libxml2-dev pkgconf
COPY . /updater
WORKDIR /updater/aria2
RUN autoreconf -i
RUN ARIA2_STATIC=yes ./configure --without-libxml2 --without-libexpat --without-sqlite3 --disable-ssl --enable-libaria2 --without-zlib --without-libcares --enable-static=yes
RUN make clean && make -j`nproc`
WORKDIR /updater
ENV PATH=/opt/Qt5.8-static-release/bin:$PATH
RUN qmake -config release
RUN make clean && make -j`nproc`
CMD cp updater /build-docker
