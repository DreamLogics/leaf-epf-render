#-------------------------------------------------
#
# Project created by QtCreator 2013-09-03T14:12:53
#
#-------------------------------------------------

QT       += gui

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

macx:INCLUDEPATH += /usr/local/include

#unix|win32: LIBS += -lavcodec
#unix|win32: LIBS += -lavformat
#unix|win32: LIBS += -lavutil
#unix|win32: LIBS += -lswscale

macx: LIBS += /usr/local/lib/libavcodec.a
macx: LIBS += /usr/local/lib/libavformat.a
macx: LIBS += /usr/local/lib/libavutil.a
macx: LIBS += /usr/local/lib/libswscale.a

macx: LIBS += /usr/lib/libbz2.dylib
macx: LIBS += /usr/lib/libz.dylib
macx: LIBS += /usr/local/lib/libvpx.a
