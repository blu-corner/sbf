TEMPLATE = app
TARGET = sbf-publisher

include (../../build.pri)

SOURCES += \
    sbfPublisher.c

LIBS += \
    -lsbfcore \
    -lsbfcommon \
    -lsbftransport

windows {
    CONFIG -= staticlib
    CONFIG += console static
}

target.path = $$top_build/bin
INSTALLS += target
