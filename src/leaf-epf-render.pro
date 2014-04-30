#-------------------------------------------------
#
# Project created by QtCreator 2014-04-30T16:25:43
#
#-------------------------------------------------

QT       += opengl script

TARGET = leaf-epf-render
TEMPLATE = lib

DEFINES += LEAFEPFRENDER_LIBRARY

SOURCES += leafepfrender.cpp \
    renderer.cpp \
    document.cpp

HEADERS += leafepfrender.h\
        leaf-epf-render_global.h \
    renderer.h \
    document.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
