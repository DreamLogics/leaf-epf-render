#-------------------------------------------------
#
# Project created by QtCreator 2013-09-03T14:12:53
#
#-------------------------------------------------

QT       -= gui

TARGET = book
TEMPLATE = lib

DEFINES += BOOK_LIBRARY

SOURCES += book.cpp \
    cfootnoteobject.cpp

HEADERS += book.h\
        book_global.h \
    cfootnoteobject.h

linux-g++:LIBS += -L$$PWD/../../build-leaf-epf-render-Desktop_Qt_5_1_1_GCC_64bit-Debug/ -lleaf-epf-render
macx:LIBS += -L$$PWD/../../build-leaf-epf-render-Desktop_Qt_5_1_0_clang_64bit-Debug/ -lleaf-epf-render

INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src
