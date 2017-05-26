QT += qml quick network widgets

include(fluid/fluid.pri)

CONFIG += c++11

HEADERS += ariadownloader.h \
    downloadtimecalculator.h \
    downloadworker.h \
    system.h \
    settings.h \
    qmldownloader.h \
    currentversionfetcher.h


SOURCES += ariadownloader.cpp \
    downloadtimecalculator.cpp \
    downloadworker.cpp \
    main.cpp \
    qmldownloader.cpp \
    settings.cpp \
    currentversionfetcher.cpp

mac {
  SOURCES += osx.cpp
} linux {
  SOURCES += unix.cpp
} win32 {
  SOURCES += win.cpp
}

DEFINES += QUAZIP_BUILD
DEFINES += QUAZIP_STATIC
include(quazip/quazip/quazip.pri)


RESOURCES += qml.qrc

win32:LIBS += -lz $$PWD/aria2/src/.libs/libaria2.a
unix:LIBS += -lz "-L$$PWD/aria2/src/.libs" -laria2

win32:RC_FILE = updater.rc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += fluid/src/imports/controls/qmldir

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    resources/unvanquished.desktop \
    resources/unvanquished.png
