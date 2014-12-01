TEMPLATE = lib
TARGET = sbfproperties

include (../build.pri)

HEADERS += \
    sbfProperties.h
SOURCES += \
    sbfProperties.c

install_headers.path = $$top_build/include
install_headers.files = \
    sbfProperties.h

target.path = $$top_build/lib
INSTALLS += install_headers target
