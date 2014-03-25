TEMPLATE = app
TARGET = sbf-replier

include (../../build.pri)

SOURCES += \
    sbfReplier.c

LIBS += \
    -lsbfcore \
    -lsbfcommon \
    -lsbfrequestreply \
    -lsbftransport

windows {
    CONFIG -= staticlib
    CONFIG += console static
}

target.path = $$top_build/bin
INSTALLS += target
