# updater
Autoupdates Unvanquished using Unvanquished's CDN

## Initializing submodules
```
git submodule update --init
```

## Build for Linux (without Docker)

### Build aria2
Before building the updater itself you need to build aria2 library
```
cd aria2
../build-aria.sh
cd ..
```

### Build updater
You need at least Qt 6.8.

```
mkdir build; cd build
cmake -GNinja .. -DCMAKE_BUILD_TYPE=Release
ninja
```

## Build Linux version in docker
```
docker build -t unvlauncher-linux .
docker run -v `pwd`/build-docker:/build-docker -u `id -u $USER` unvlauncher-linux
```
Search for **updater** (or **updater-nonstripped**) in build-docker directory.

## Build Windows version in docker

If the code is checked out on a Windows host, you must ensure that the line endings in the aria2 submodule are LF. The symptom of CRLF newlines is `libtoolize: AC_CONFIG_MACRO_DIR([m4]) conflicts with ACLOCAL_AMFLAGS=-I m4`. To reset the newlines:

```
cd aria2
git config core.autocrlf input
git rm -r --cached . && git reset --hard
```

The first line below runs the Docker build for Windows. The last 3 lines are to copy the result out of the container.

```
docker build -t unvlauncher-win -f Dockerfile.win .
docker create --name unvlauncher-win unvlauncher-win
docker cp unvlauncher-win:/build/updater.exe ./build-docker
docker rm unvlauncher-win

```
Search for **updater.exe** in build-docker directory.

## Build Mac version natively

### Build Qt
```
updaterdir=$PWD
cd ~/buildqt
$updaterdir/build-qt.sh macos
```

### Build aria2
```
brew install autoconf automake libtool pkg-config gettext
cd aria2
# If building on M1, target x86 by running in Rosetta: arch -x86_64 ../build-aria.sh
# (the --target option to configure doesn't seem to have any effect)
CFLAGS=-Os CXXFLAGS=-Os ../build-aria.sh
cd ..
```

### Build updater
```
mkdir build; cd build
cmake .. -G Ninja -DCMAKE_FIND_ROOT_PATH=$HOME/buildqt/qt_macos
ninja
```

## License

The Unvanquished Updater as a whole is available under GPLv3. Source files in this
repository are licensed as GPL version 3 or greater, unless the file specifies
otherwise.

Dependencies have the following licenses:
- Qt: GPLv2 or GPLv3 or LGPLv3
- Quazip: LGPL version 2.1 or greater
- Fluid: Mozilla Public License 2.0
- Aria2: GPL version 2 or greater
- OpenSSL 3.x (Linux build only): Apache License 2.0
- ExecInExplorer.cpp: MIT License
