#-------------------------------------------------
#
# Project created by QtCreator 2012-11-09T13:59:40
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = epf-create
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    coepfdocumentwriter.cpp \
    czlib.cpp

HEADERS += \
    coepfdocumentwriter.h \
    czlib.h