TEMPLATE = lib
TARGET = sbfcommon

include (../build.pri)

HEADERS += \
    sbfBuffer.h \
    sbfBufferInline.h \
    sbfCommon.h \
    sbfFatal.h \
    sbfInterface.h \
    sbfKeyValue.h \
    sbfKeyValuePrivate.h \
    sbfLog.h \
    sbfMemory.h \
    sbfPerfCounter.h \
    sbfPool.h \
    sbfUdpMulticast.h \
    sbfTcpConnection.h \
    sbfTcpListener.h \
    sbfVersion.h
SOURCES += \
    sbfBuffer.c \
    sbfFatal.c \
    sbfInterface.c \
    sbfKeyValue.c \
    sbfLog.c \
    sbfMemory.c \
    sbfPerfCounter.c \
    sbfPool.c \
    sbfTcpConnection.c \
    sbfTcpListener.c \
    sbfUdpMulticast.c
unix {
    HEADERS += sbfCommonLinux.h
    SOURCES += sbfCommonLinux.c
}
windows {
    HEADERS += sbfCommonWin32.h
    SOURCES += sbfCommonWin32.c getopt.c
}

install_headers.path = $$top_build/include
install_headers.files = \
    sbfBuffer.h \
    sbfBufferInline.h \
    sbfCommon.h \
    sbfFatal.h \
    sbfInterface.h \
    sbfKeyValue.h \
    sbfLog.h \
    sbfMemory.h \
    sbfPerfCounter.h \
    sbfPool.h \
    sbfPoolInline.h \
    sbfRefCount.h \
    sbfTcpConnection.h \
    sbfTcpListener.h \
    sbfUdpMulticast.h \
    sbfVersion.h \
    syshash.h \
    sysqueue.h \
    systree.h
unix {
    install_headers.files += sbfCommonLinux.h
}
windows {
    install_headers.files += sbfCommonWin32.h
}

target.path = $$top_build/lib

INSTALLS += install_headers target
