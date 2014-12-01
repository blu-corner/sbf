TEMPLATE = lib
TARGET = sbftransport

include (../build.pri)

HEADERS += \
    sbfBatch.h \
    sbfBatchPrivate.h \
    sbfHandler.h \
    sbfPub.h \
    sbfPubPrivate.h \
    sbfSub.h \
    sbfSubPrivate.h \
    sbfTport.h \
    sbfTportPrivate.h \
    sbfTopic.h \
    sbfTopicPrivate.h
SOURCES += \
    sbfBatch.c \
    sbfHandler.c \
    sbfPub.c \
    sbfSub.c \
    sbfTport.c \
    sbfTportFragments.c \
    sbfTportWeight.c \
    sbfTopic.c

install_headers.path = $$top_build/include
install_headers.files = \
    sbfBatch.h \
    sbfPub.h \
    sbfSub.h \
    sbfTport.h \
    sbfTopic.h

target.path = $$top_build/lib
INSTALLS += install_headers target
