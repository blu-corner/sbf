TEMPLATE = lib
TARGET = sbftcpmeshhandler

include (../../build.pri)

HEADERS += \
    sbfTcpMeshHandler.h
SOURCES += \
    sbfTcpMeshHandler.c

LIBS += \
    -lsbfcommon \
    -lsbfcore \
    -lsbftransport

target.path = $$top_build/lib
INSTALLS += target
