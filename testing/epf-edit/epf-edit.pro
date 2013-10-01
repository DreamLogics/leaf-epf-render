#-------------------------------------------------
#
# Project created by QtCreator 2013-10-01T14:34:05
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = epf-edit
TEMPLATE = app


SOURCES += main.cpp\
        epfedit.cpp \
    newdoc.cpp \
    epfeditview.cpp \
    sectionview.cpp \
    layoutsview.cpp

HEADERS  += epfedit.h \
    newdoc.h \
    epfeditview.h \
    sectionview.h \
    layoutsview.h

FORMS    += epfedit.ui \
    newdoc.ui \
    sectionview.ui \
    layoutsview.ui
