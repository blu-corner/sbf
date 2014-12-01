TEMPLATE = lib
TARGET = sbfcommon

include (../build.pri)

HEADERS += \
    sbfArguments.h \
    sbfAtomic.h \
    sbfBuffer.h \
    sbfBufferInline.h \
    sbfCacheFile.h \
    sbfCacheFilePrivate.h \
    sbfCommon.h \
    sbfCsvParser.h \
    sbfDeque.h \
    sbfDequePrivate.h \
    sbfFatal.h \
    sbfGuid.h \
    sbfHugePages.h \
    sbfInterface.h \
    sbfKeyValue.h \
    sbfKeyValuePrivate.h \
    sbfLog.h \
    sbfLogPrivate.h \
    sbfMemory.h \
    sbfNumberString.h \
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
    sbfArguments.c \
    sbfBuffer.c \
    sbfCacheFile.c \
    sbfCsvParser.c \
    sbfDeque.c \
    sbfFatal.c \
    sbfGuid.c \
    sbfHugePages.c \
    sbfInterface.c \
    sbfKeyValue.c \
    sbfLog.c \
    sbfMemory.c \
    sbfNumberString.c \
    sbfPerfCounter.c \
    sbfPool.c \
    sbfTcpConnection.c \
    sbfTcpListener.c \
    sbfUdpMulticast.c
unix:!macx {
HEADERS += \
    sbfCommonLinux.h
SOURCES += \
    sbfCommonLinux.c \
    fgetln.c \
    strlcat.c \
    strlcpy.c
}
macx {
HEADERS += \
    sbfCommonDarwin.h
SOURCES += \
    sbfCommonDarwin.c
}
windows {
HEADERS += \
    sbfCommonWin32.h
SOURCES += \
    sbfCommonWin32.c \
    getopt.c \
    fgetln.c \
    strlcat.c \
    strlcpy.c
}

install_headers.path = $$top_build/include
install_headers.files = \
    sbfArguments.h \
    sbfAtomic.h \
    sbfBuffer.h \
    sbfBufferInline.h \
    sbfCacheFile.h \
    sbfCommon.h \
    sbfCsvParser.h \
    sbfDeque.h \
    sbfFatal.h \
    sbfGuid.h \
    sbfHugePages.h \
    sbfInterface.h \
    sbfKeyValue.h \
    sbfLog.h \
    sbfMemory.h \
    sbfNumberString.h \
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
unix:!macx {
install_headers.files += \
    sbfCommonLinux.h
}
macx {
install_headers.files += \
    sbfCommonDarwin.h
}
windows {
install_headers.files += \
    sbfCommonWin32.h
}

target.path = $$top_build/lib
INSTALLS += install_headers target
