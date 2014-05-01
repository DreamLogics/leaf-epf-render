#-------------------------------------------------
#
# Project created by QtCreator 2014-04-30T16:25:43
#
#-------------------------------------------------

QT       += opengl script

TARGET = leaf-epf-render
TEMPLATE = lib

include(EPFText/src/EPFText.pri)

DEFINES += LEAFEPFRENDER_LIBRARY

SOURCES += leafepfrender.cpp \
    document.cpp \
    baseobject.cpp \
    documentreader.cpp \
    layout.cpp \
    section.cpp \
    scrollarea.cpp \
    object-types/textobject.cpp \
    object-types/blockobject.cpp \
    object-types/videoobject.cpp

HEADERS += leafepfrender.h\
        leaf-epf-render_global.h \
    document.h \
    baseobject.h \
    documentreader.h \
    layout.h \
    section.h \
    scrollarea.h \
    object-types/textobject.h \
    object-types/blockobject.h \
    object-types/videoobject.h \
    baseobject_private.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
