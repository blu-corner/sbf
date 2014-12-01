TEMPLATE = lib
TARGET = sbfrequestreply

include (../build.pri)

HEADERS += \
    sbfRequestPub.h \
    sbfRequestReply.h \
    sbfRequestSub.h
SOURCES += \
    sbfRequestPub.c \
    sbfRequestReply.c \
    sbfRequestSub.c

install_headers.path = $$top_build/include
install_headers.files = \
    sbfRequestPub.h \
    sbfRequestReply.h \
    sbfRequestSub.h

target.path = $$top_build/lib
INSTALLS += install_headers target
