# updater
Autoupdates Unvanquished using Unvanquished's CDN

## Initializing aria2 submodule
```
git submodule init && git submodule update
```

## Build Instructions (CMake)
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

## Build Instructions (qmake)
```
QT_SELECT=5 qmake -config release
make -j4
```

## Build Linux version in docker
```
docker build -t updater .
docker run -v `pwd`/build-docker:/build-docker -u `id -u $USER` updater
strip build-docker/updater # Optionally strip binary to reduce its size
```
Search for updater in build-docker directory.
