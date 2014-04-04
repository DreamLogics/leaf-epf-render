#-------------------------------------------------
#
# Project created by QtCreator 2014-04-04T13:49:41
#
#-------------------------------------------------

QT       += core gui opengl script svg xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = epf-reader
TEMPLATE = app

include(../../src/leaf-epf-render.pri)
#include(../../modules/media/media.pri)
include(../../modules/gui/gui.pri)
include(../../modules/tech/tech.pri)


SOURCES += main.cpp\
        epfreader.cpp \
    cdesktopdevice.cpp

HEADERS  += epfreader.h \
    cdesktopdevice.h

FORMS    += epfreader.ui
