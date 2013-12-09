TEMPLATE = lib
TARGET = sbfudphandler

include (../../build.pri)

HEADERS += \
    sbfUdpHandler.h
SOURCES += \
    sbfUdpHandler.c

target.path = $$top_build/lib

INSTALLS += target
