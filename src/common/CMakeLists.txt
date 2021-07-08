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
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfVersion.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfShmRingBuffer.h"
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
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfShmRingBuffer.c")

set (LIBS ${EVENT_LIB}
  ${EVENT_CORE_LIB}
  ${CMAKE_THREAD_LIBS_INIT}
  ${CMAKE_DL_LIBS})

if(APPLE)
  set (INSTALL_HEADERS "${INSTALL_HEADERS}"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfCommonDarwin.h")
  set (SOURCES "${SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfCommonDarwin.c")
  set (LIBS "${LIBS}"
    ${EVENT_PTHREAD_LIB})
elseif(WIN32)
  set (INSTALL_HEADERS "${INSTALL_HEADERS}"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfCommonWin32.h"
    "${CMAKE_SOURCE_DIR}/src/thirdparty/getopt/getopt.h")
  set (SOURCES "${SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfCommonWin32.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/fgetln.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/strlcat.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/strlcpy.c"
    "${CMAKE_SOURCE_DIR}/src/thirdparty/getopt/getopt.c")
  set (LIBS "${LIBS}"
    ${EVENT_CORE_LIB}
    ${EVENT_EXTRA_LIB}
    Ws2_32.lib
    Iphlpapi.lib
    Pathcch.lib)
else()
  set (INSTALL_HEADERS "${INSTALL_HEADERS}"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfCommonLinux.h")
  set (SOURCES "${SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfCommonLinux.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/fgetln.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/strlcat.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/strlcpy.c")
  set (LIBS "${LIBS}"
    ${EVENT_PTHREAD_LIB})
endif()

add_library (sbfcommon SHARED ${SOURCES})
target_link_libraries (sbfcommon ${LIBS})

if (NOT EVENT_LIB)
    add_dependencies (sbfcommon libevent)
endif()

install (TARGETS sbfcommon
         EXPORT sbfcommon
         COMPONENT library
         RUNTIME DESTINATION lib
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
install (FILES ${INSTALL_HEADERS} DESTINATION include/sbf)
