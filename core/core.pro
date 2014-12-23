TEMPLATE = lib
TARGET = sbfcore

include (../build.pri)

HEADERS += \
    sbfEvent.h \
    sbfEventPrivate.h \
    sbfHiResTimer.h \
    sbfHiResTimerPrivate.h \
    sbfMw.h \
    sbfMwPrivate.h \
    sbfQueue.h \
    sbfQueuePrivate.h \
    sbfQueueLinux.h \
    sbfQueueWin32.h \
    sbfTimer.h \
    sbfTimerPrivate.h
SOURCES += \
    sbfEvent.c \
    sbfHiResTimer.c \
    sbfMw.c \
    sbfQueue.c \
    sbfTimer.c

LIBS += \
    -lsbfcommon

install_headers.path = $$top_build/include
install_headers.files = \
    sbfEvent.h \
    sbfHiResTimer.h \
    sbfMw.h \
    sbfQueue.h \
    sbfTimer.h

target.path = $$top_build/lib
INSTALLS += install_headers target
