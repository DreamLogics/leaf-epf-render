QT       += core gui opengl script svg

TARGET = epf-view
TEMPLATE = app

include(../../src/leaf-epf-render.pri)

SOURCES += \
    main.cpp
