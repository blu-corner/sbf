TEMPLATE = lib
TARGET = sbfcore

include (../build.pri)

HEADERS += \
    sbfEvent.h \
    sbfEventPrivate.h \
    sbfMw.h \
    sbfMwInternal.h \
    sbfMwPrivate.h \
    sbfQueue.h \
    sbfQueuePrivate.h \
    sbfQueueLinux.h \
    sbfQueueWin32.h \
    sbfTimer.h \
    sbfTimerPrivate.h
SOURCES += \
    sbfEvent.c \
    sbfMw.c \
    sbfQueue.c \
    sbfTimer.c

install_headers.path = $$top_build/include
install_headers.files = \
    sbfEvent.h \
    sbfMw.h \
    sbfQueue.h \
    sbfTimer.h

target.path = $$top_build/lib

INSTALLS += install_headers target
