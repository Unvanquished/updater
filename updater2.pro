QT += qml quick network widgets

include(fluid/fluid.pri)

CONFIG += c++11

HEADERS += ariadownloader.h \
    downloadtimecalculator.h \
    downloadworker.h \
    gamelauncher.h \
    system.h \
    settings.h \
    splashcontroller.h \
    qmldownloader.h \
    currentversionfetcher.h


SOURCES += ariadownloader.cpp \
    downloadtimecalculator.cpp \
    downloadworker.cpp \
    gamelauncher.cpp \
    main.cpp \
    qmldownloader.cpp \
    settings.cpp \
    splashcontroller.cpp \
    currentversionfetcher.cpp

mac {
  SOURCES += osx.cpp
} linux {
  SOURCES += linux.cpp
} win32 {
  SOURCES += win.cpp ExecInExplorer.cpp
}

GIT_VERSION = $$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags --abbrev=0)
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"
DEFINES += QUAZIP_BUILD
DEFINES += QUAZIP_STATIC
include(quazip/quazip/quazip.pri)


RESOURCES += qml.qrc

win32:LIBS += $$PWD/aria2/src/.libs/libaria2.a
win32:LIBS += -lcrypt32 -lsecur32 -lole32
unix:LIBS += -lz "-L$$PWD/aria2/src/.libs" -laria2

win32:RC_FILE = updater.rc

# Plain Unvanquished icon, used for the app bundle and hence the Launchpad shortcut.
# When the updater is running, the one with arrows is displayed as its icon.
mac: ICON = resources/Unvanquished.icns

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += fluid/src/imports/controls/qmldir

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Turn on warning errors since the warnings are hard to see in Docker
!mac: QMAKE_CXXFLAGS += "-Werror"

# Disables APIs deprecated by version 5.13 (love the hexadecimal!), but turns off warnings
# for stuff deprecated in 5.14+. This is because Fluid has 5.14 deprecation warnings
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050D00

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
