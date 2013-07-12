#-------------------------------------------------
#
# Project created by QtCreator 2013-06-04T15:25:08
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = leaftest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui


macx: LIBS += -L$$PWD/../leaf-epf-render/leaf-epf-render-build-desktop-Qt___qt5__Debug/ -lleaf-epf-render

INCLUDEPATH += $$PWD/../leaf-epf-render/src
DEPENDPATH += $$PWD/../leaf-epf-render/src
