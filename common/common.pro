TEMPLATE = lib
TARGET = sbfcommon

include (../build.pri)

HEADERS += \
    sbfAtomic.h \
    sbfBuffer.h \
    sbfBufferInline.h \
    sbfCacheFile.h \
    sbfCacheFilePrivate.h \
    sbfCommon.h \
    sbfCsvParser.h \
    sbfFatal.h \
    sbfGuid.h \
    sbfInterface.h \
    sbfKeyValue.h \
    sbfKeyValuePrivate.h \
    sbfLog.h \
    sbfMemory.h \
    sbfPerfCounter.h \
    sbfPool.h \
    sbfPoolInline.h \
    sbfTcpConnection.h \
    sbfTcpConnectionPrivate.h \
    sbfTcpListener.h \
    sbfTcpListenerPrivate.h \
    sbfUdpMulticast.h \
    sbfUdpMulticastPrivate.h \
    sbfVersion.h
SOURCES += \
    sbfBuffer.c \ 
    sbfCacheFile.c \
    sbfCsvParser.c \
    sbfFatal.c \
    sbfGuid.c \
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
    SOURCES += sbfCommonLinux.c fgetln.c
}
windows {
    HEADERS += sbfCommonWin32.h
    SOURCES += sbfCommonWin32.c getopt.c fgetln.c
}

install_headers.path = $$top_build/include
install_headers.files = \
    sbfAtomic.h \
    sbfBuffer.h \
    sbfBufferInline.h \
    sbfCacheFile.h \
    sbfCommon.h \
    sbfCsvParser.h \
    sbfFatal.h \
    sbfGuid.h \
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
