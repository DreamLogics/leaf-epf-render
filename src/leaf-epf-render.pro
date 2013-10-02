#-------------------------------------------------
#
# Project created by QtCreator 2013-04-19T14:35:43
#
#-------------------------------------------------

QT       += opengl script svg

TARGET = leaf-epf-render
TEMPLATE = lib

DEFINES += LEAFEPFRENDER_LIBRARY

SOURCES += cdocument.cpp \
    cepfview.cpp \
    csection.cpp \
    coverlay.cpp \
    clayer.cpp \
    cbaseobject.cpp \
    cepfdocumentreader.cpp \
    canimation.cpp \
    canimframe.cpp \
    object_types/ctextobject.cpp \
    object_types/cscriptobject.cpp \
    object_types/cimageobject.cpp \
    object_types/cblockobject.cpp \
    cunsupportedobject.cpp \
    clayout.cpp \
    css/css_style.cpp \
    pugixml/src/pugixml.cpp \
    czlib.cpp \
    css/css_painters.cpp \
    epfevent.cpp \
    object_types/clabelobject.cpp \
    device.cpp \
    css/css_animation.cpp \
    canimator.cpp \
    cepfjs.cpp \
    css/css_transition.cpp

HEADERS += leaf-epf-render_global.h \
    cdocument.h \
    cepfview.h \
    idevice.h \
    csection.h \
    coverlay.h \
    clayer.h \
    cbaseobject.h \
    cepfdocumentreader.h \
    canimation.h \
    canimframe.h \
    object_types/ctextobject.h \
    object_types/cscriptobject.h \
    object_types/cimageobject.h \
    object_types/cblockobject.h \
    cunsupportedobject.h \
    iepfobjectfactory.h \
    clayout.h \
    css/css_style.h \
    css/istyleeffect.h \
    pugixml/src/pugixml.hpp \
    pugixml/src/pugiconfig.hpp \
    czlib.h \
    css/css_painters.h \
    css/css_default.h \
    epfevent.h \
    object_types/clabelobject.h \
    css/css_animation.h \
    canimator.h \
    cepfjs.h \
    css/css_transition.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xED1308CE
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = leaf-epf-render.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

OTHER_FILES += \
    license.txt
