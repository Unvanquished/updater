
QT += network widgets gui core
TARGET = updater
TEMPLATE = app

HEADERS += newsfetcher.h \
  currentversionfetcher.h \
  ariadownloader.h \
  downloadworker.h \
  system.h \
  settingsdialog.h \
  downloadtimecalculator.h \
  mainwindow.h
SOURCES += newsfetcher.cpp \
  currentversionfetcher.cpp \
  ariadownloader.cpp \
  downloadworker.cpp \
  main.cpp \
  settingsdialog.cpp \
  downloadtimecalculator.cpp \
  mainwindow.cpp

mac {
  SOURCES += osx.cpp
} unix {
  SOURCES += unix.cpp
} win32 {
  SOURCES += win.cpp
}

CONFIG += c++11

DEFINES += QUAZIP_BUILD
DEFINES += QUAZIP_STATIC
include(quazip/quazip.pri)

win32:LIBS += -lz aria2/src/.libs/libaria2.a
unix:LIBS += -lz "-Laria2/src/.libs" -laria2
RESOURCES += resources.qrc
FORMS += mainwindow.ui settingsdialog.ui
