#-------------------------------------------------
#
# Project created by QtCreator 2013-09-03T14:12:53
#
#-------------------------------------------------

QT       -= gui

TARGET = media
TEMPLATE = lib

DEFINES += MEDIA_LIBRARY

SOURCES += cavdecoder.cpp \
    cvideoobject.cpp \
    caudioobject.cpp

HEADERS += cavdecoder.h \
    cvideoobject.h \
    caudioobject.h

linux-g++:LIBS += -L$$PWD/../../build-leaf-epf-render-Desktop_Qt_5_1_1_GCC_64bit-Debug/ -lleaf-epf-render
macx:LIBS += -L$$PWD/../../build-leaf-epf-render-Desktop_Qt_5_1_0_clang_64bit-Debug/ -lleaf-epf-render

INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src

unix|win32: LIBS += -lavcodec
unix|win32: LIBS += -lavformat
unix|win32: LIBS += -lavutil
unix|win32: LIBS += -lswscale
