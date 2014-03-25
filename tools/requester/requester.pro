TEMPLATE = app
TARGET = sbf-requester

include (../../build.pri)

SOURCES += \
    sbfRequester.c

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
