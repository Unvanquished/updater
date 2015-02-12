# updater
Autoupdates Unvanquished using Unvanquished's CDN

## Build Instructions
```
cd aria2
autoreconf -i
ARIA2_STATIC=yes ./configure --without-libxml2 --without-libexpat --without-sqlite3 --disable-ssl --enable-libaria2 --without-zlib --without-libcares --enable-static=yes
make -j4
cd ..
mkdir build
cd build
cmake ..
make -j4
```
