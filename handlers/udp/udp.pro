TEMPLATE = lib
TARGET = sbfudphandler

include (../../build.pri)

HEADERS += \
    sbfUdpHandler.h
SOURCES += \
    sbfUdpHandler.c

windows {
    CONFIG -= staticlib
    target.path = $$top_build/bin
    
    LIBS += \
        sbfcore.lib \
        sbfcommon.lib \
        sbftransport.lib
}

unix {
    target.path = $$top_build/lib
}

INSTALLS += target
