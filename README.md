# updater
Autoupdates Unvanquished using Unvanquished's CDN

## Initializing submodules
```
git submodule init && git submodule update
```

## Build aria2
Before building the updater itself you need to build aria2 library
```
cd aria2
autoreconf -i
./configure --without-libxml2 --without-libexpat --without-sqlite3 --enable-libaria2 --without-zlib --without-libcares --enable-static=yes ARIA2_STATIC=yes --without-libssh2 --disable-websocket
make -j4
cd ..
```

## Build updater
Note: you need Qt 5.8 at least.
```
QT_SELECT=5 qmake -config release
make -j4
```

## Build Linux version in docker
```
docker build -t updater2 .
docker run -v `pwd`/build-docker:/build-docker -u `id -u $USER` updater2
strip build-docker/updater2 # Optionally strip binary to reduce its size
```
Search for **updater2** in build-docker directory.

## Build Windows version in docker

If the code is checked out on a Windows host, you must ensure that the line endings in the aria2 submodule are LF. The symptom of CRLF newlines is `libtoolize: AC_CONFIG_MACRO_DIR([m4]) conflicts with ACLOCAL_AMFLAGS=-I m4`. To reset the newlines:

```
cd aria2
git config core.autocrlf input
git rm --cached . && git reset --hard
```

The first line below runs the Docker build for Windows. The last 3 lines are to copy the result out of the container.

```
docker build -t updater2-win -f Dockerfile.win .
docker create --name updater2-win updater2-win
docker cp updater2-win:/build/release/updater2.exe ./build-docker
docker rm updater2-win

```
Search for **updater2.exe** in build-docker directory.
