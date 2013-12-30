TEMPLATE = lib
TARGET = sbftcpmeshhandler

include (../../build.pri)

HEADERS += \
    sbfTcpMeshHandler.h
SOURCES += \
    sbfTcpMeshHandler.c

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
