#-------------------------------------------------
#
# Project created by QtCreator 2014-03-21T15:58:16
#
#-------------------------------------------------

QT       -= gui

TARGET = tech
TEMPLATE = lib

DEFINES += TECH_LIBRARY

include(tech.pri)

unix {
    target.path = /usr/lib
    INSTALLS += target
}
