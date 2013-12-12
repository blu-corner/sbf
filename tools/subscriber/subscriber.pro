TEMPLATE = app
TARGET = sbf-subscriber

include (../../build.pri)

SOURCES += \
    sbfSubscriber.c

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
