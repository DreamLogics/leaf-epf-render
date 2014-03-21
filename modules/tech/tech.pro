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

HEADERS += \
    mathml/qwt_mathml_text_engine.h \
    mathml/qwt_mml_document.h \
    mathml/qwt_mml_entity_table.h

SOURCES += \
    mathml/qwt_mathml_text_engine.cpp \
    mathml/qwt_mml_document.cpp \
    mathml/qwt_mml_entity_table.cpp
