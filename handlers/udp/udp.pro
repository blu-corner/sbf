TEMPLATE = lib
TARGET = sbfudphandler

include (../../build.pri)

HEADERS += \
    sbfUdpHandler.h
SOURCES += \
    sbfUdpHandler.c

LIBS += \
    -lsbfcommon \
    -lsbfcommonhandler \
    -lsbfcore \
    -lsbftransport

target.path = $$top_build/lib
INSTALLS += target
