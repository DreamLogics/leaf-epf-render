#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T17:39:22
#
#-------------------------------------------------

QT       -= gui

TARGET = gui
TEMPLATE = lib

DEFINES += GUI_LIBRARY

include(gui.pri)

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
