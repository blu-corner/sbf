set (INSTALL_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfArguments.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfAtomic.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfBuffer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfBufferInline.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfCacheFile.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfCommon.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfCsvParser.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfDeque.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfFatal.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfGuid.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfHugePages.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfInterface.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfKeyValue.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfLog.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfLogFile.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfMemory.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfNumberString.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfPath.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfPerfCounter.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfPool.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfPoolInline.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfProperties.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfRefCount.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTcpConnection.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTcpListener.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfUdpMulticast.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfVersion.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/syshash.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sysqueue.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/systree.h")

set (SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfArguments.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfBuffer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfCacheFile.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfCsvParser.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfDeque.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfFatal.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfGuid.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfHugePages.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfInterface.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfKeyValue.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfLog.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfLogFile.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfMemory.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfNumberString.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfPath.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfPerfCounter.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfPool.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfProperties.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTcpConnection.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTcpListener.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfUdpMulticast.c")

if (UNIX)
    set (INSTALL_HEADERS "${INSTALL_HEADERS}"
        "${CMAKE_CURRENT_SOURCE_DIR}/sbfCommonLinux.h")
    set (SOURCES "${SOURCES}"
        "${CMAKE_CURRENT_SOURCE_DIR}/sbfCommonLinux.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/fgetln.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/strlcat.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/strlcpy.c")
endif (UNIX)

add_library (sbfcommon SHARED ${SOURCES})
install (TARGETS sbfcommon
         LIBRARY DESTINATION lib)

install (FILES ${INSTALL_HEADERS} DESTINATION include)
