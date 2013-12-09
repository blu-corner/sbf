TEMPLATE = app
TARGET = sbf-publisher

include (../../build.pri)

SOURCES += \
    sbfPublisher.c

LIBS += \
    -lsbfcore \
    -lsbfcommon \
    -lsbftransport

target.path = $$top_build/bin
INSTALLS += target
