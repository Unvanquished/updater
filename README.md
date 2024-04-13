# updater
Autoupdates Unvanquished using Unvanquished's CDN

## Initializing submodules
```
git submodule init && git submodule update
```

## Build for Linux (without Docker)

### Build aria2
Before building the updater itself you need to build aria2 library
```
cd aria2
autoreconf -i
./configure --without-libxml2 --without-libexpat --without-sqlite3 --enable-libaria2 --without-zlib --without-libcares --enable-static=yes ARIA2_STATIC=yes --without-libssh2 --disable-websocket --disable-nls --with-openssl
make -j4
cd ..
```

### Build updater
Note: you need Qt 5.8 at least.
```
QT_SELECT=5 qmake -config release
make -j4
```

## Build Linux version in docker
```
docker build -t updater2 .
docker run -v `pwd`/build-docker:/build-docker -u `id -u $USER` updater2
```
Search for **updater2** (or **updater2-nonstripped**) in build-docker directory.

## Build Windows version in docker

If the code is checked out on a Windows host, you must ensure that the line endings in the aria2 submodule are LF. The symptom of CRLF newlines is `libtoolize: AC_CONFIG_MACRO_DIR([m4]) conflicts with ACLOCAL_AMFLAGS=-I m4`. To reset the newlines:

```
cd aria2
git config core.autocrlf input
git rm -r --cached . && git reset --hard
```

The first line below runs the Docker build for Windows. The last 3 lines are to copy the result out of the container.

```
docker build -t updater2-win -f Dockerfile.win .
docker create --name updater2-win updater2-win
docker cp updater2-win:/build/release/updater2.exe ./build-docker
docker rm updater2-win

```
Search for **updater2.exe** in build-docker directory.

## Build Mac version natively

### Build Qt
```
curl -LO https://download.qt.io/archive/qt/5.14/5.14.2/single/qt-everywhere-src-5.14.2.tar.xz
tar -xzf qt-everywhere-src-5.14.2.tar.xz
cd qt-everywhere-src-5.14.2
MODULES=qtbase,qtquickcontrols,qtquickcontrols2,qtsvg,qtgraphicaleffects
# Default install location (--prefix): /usr/local/Qt-5.14.2
./configure -opensource -confirm-license -release -optimize-size -no-shared -no-framework -static --c++std=14 -nomake tests -nomake tools -nomake examples
cat <(echo '#include <CoreGraphics/CGColorSpace.h>') qtbase/src/plugins/platforms/cocoa/qiosurfacegraphicsbuffer.h > tmp && mv tmp qtbase/src/plugins/platforms/cocoa/qiosurfacegraphicsbuffer.h
eval make -j`sysctl -n hw.logicalcpu` module-{$MODULES}
echo $?  # If nonzero, repeat previous command
eval sudo make module-{$MODULES}-install_subtargets
```

### Build aria2
```
brew install autoconf automake libtool pkg-config gettext
# If building on M1, target x86 by running in Rosetta: arch -x86_64 ./mac-build-aria.sh
# (the --target option to configure doesn't seem to have any effect)
./mac-build-aria.sh
```

### Build updater
```
mkdir build; cd build
/usr/local/Qt-5.14.2/bin/qmake -config release ..
make
```

## License

The Unvanquished Updater as a whole is available under GPLv3. Source files in this
repository are licensed as GPL version 3 or greater, unless the file specifies
otherwise.

Dependencies have the following licenses:
- Qt: GPLv2 or GPLv3 or GPLv3
- Quazip: LGPL version 2.1 or greater
- Fluid: Mozilla Public License 2.0
- ExecInExplorer.cpp: MIT License
