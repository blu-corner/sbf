set (INSTALL_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfEvent.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfHiResTimer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfMw.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfQueue.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTimer.h"
    )

set (SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfEvent.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfHiResTimer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfMw.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfQueue.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTimer.c"
    )

set(LIBS sbfcommon ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} ${EVENT_LIB})
if(WIN32)
  set (LIBS "${LIBS}"
    ${EVENT_CORE_LIB}
    ${EVENT_EXTRA_LIB})
else()
  set (LIBS "${LIBS}"
    ${EVENT_PTHREAD_LIB})
endif()
  
add_library (sbfcore SHARED ${SOURCES})
target_link_libraries (sbfcore ${LIBS})

if (NOT EVENT_LIB)
    add_dependencies (sbfcore libevent)
endif()

install (TARGETS sbfcore
         EXPORT sbfcore
         RUNTIME DESTINATION lib
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
install (FILES ${INSTALL_HEADERS} DESTINATION include/sbf)
