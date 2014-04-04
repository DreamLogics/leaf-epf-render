# leaf EPF

INCLUDEPATH += $$PWD

SOURCES += $$PWD/cdocument.cpp \
    $$PWD/cepfview.cpp \
    $$PWD/csection.cpp \
    $$PWD/coverlay.cpp \
    $$PWD/clayer.cpp \
    $$PWD/cbaseobject.cpp \
    $$PWD/cepfdocumentreader.cpp \
    $$PWD/canimation.cpp \
    $$PWD/canimframe.cpp \
    $$PWD/object_types/ctextobject.cpp \
    $$PWD/object_types/cscriptobject.cpp \
    $$PWD/object_types/cimageobject.cpp \
    $$PWD/object_types/cblockobject.cpp \
    $$PWD/object_types/cscrollareaobject.cpp \
    $$PWD/cunsupportedobject.cpp \
    $$PWD/clayout.cpp \
    $$PWD/css/css_style.cpp \
    $$PWD/pugixml/src/pugixml.cpp \
    $$PWD/css/css_painters.cpp \
    $$PWD/epfevent.cpp \
    $$PWD/object_types/clabelobject.cpp \
    $$PWD/device.cpp \
    $$PWD/css/css_animation.cpp \
    $$PWD/canimator.cpp \
    $$PWD/cepfjs.cpp \
    $$PWD/css/css_transition.cpp \
    $$PWD/cscrollarea.cpp

HEADERS += $$PWD/leaf-epf-render_global.h \
    $$PWD/cdocument.h \
    $$PWD/cepfview.h \
    $$PWD/idevice.h \
    $$PWD/csection.h \
    $$PWD/coverlay.h \
    $$PWD/clayer.h \
    $$PWD/cbaseobject.h \
    $$PWD/cepfdocumentreader.h \
    $$PWD/canimation.h \
    $$PWD/canimframe.h \
    $$PWD/object_types/ctextobject.h \
    $$PWD/object_types/cscriptobject.h \
    $$PWD/object_types/cimageobject.h \
    $$PWD/object_types/cblockobject.h \
    $$PWD/object_types/cscrollareaobject.h \
    $$PWD/cunsupportedobject.h \
    $$PWD/iepfobjectfactory.h \
    $$PWD/clayout.h \
    $$PWD/css/css_style.h \
    $$PWD/css/istyleeffect.h \
    $$PWD/pugixml/src/pugixml.hpp \
    $$PWD/pugixml/src/pugiconfig.hpp \
    $$PWD/css/css_painters.h \
    $$PWD/css/css_default.h \
    $$PWD/epfevent.h \
    $$PWD/object_types/clabelobject.h \
    $$PWD/css/css_animation.h \
    $$PWD/canimator.h \
    $$PWD/cepfjs.h \
    $$PWD/css/css_transition.h \
    $$PWD/cscrollarea.h

OTHER_FILES += \
    $$PWD/license.txt

LIBS += -lQtLzma
