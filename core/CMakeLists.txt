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

add_library (sbfcore SHARED ${SOURCES})

if(WIN32)
    target_link_libraries (sbfcore event_core event event_extra ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
else()
    target_link_libraries (sbfcore event event_pthreads ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
endif()

install (TARGETS sbfcore
         EXPORT sbfcore
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
install (FILES ${INSTALL_HEADERS} DESTINATION include/sbf)
