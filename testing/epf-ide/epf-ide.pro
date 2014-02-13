#-------------------------------------------------
#
# Project created by QtCreator 2014-02-13T11:27:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = epf-ide
TEMPLATE = app


SOURCES += main.cpp\
        epfide.cpp \
    cprojectviewitem.cpp \
    cepfhl.cpp

HEADERS  += epfide.h \
    cprojectviewitem.h \
    cepfhl.h

FORMS    += epfide.ui
