#-------------------------------------------------
#
# Project created by QtCreator 2014-12-20T16:11:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QMAKE_CXXFLAGS += -std=c++11

TARGET = updater
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ariadownloader.cpp \
    downloadworker.cpp

HEADERS  += mainwindow.h \
    ariadownloader.h \
    downloadworker.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/aria2/src/.libs/release/ -laria2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/aria2/src/.libs/debug/ -laria2
else:unix: LIBS += -L$$PWD/aria2/src/.libs/ -laria2

INCLUDEPATH += $$PWD/aria2/src/.libs
DEPENDPATH += $$PWD/aria2/src/.libs
