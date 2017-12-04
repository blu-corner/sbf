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
target_link_libraries (sbfcore sbfcommon event event_pthreads pthread dl)

install (TARGETS sbfcore
         LIBRARY DESTINATION lib)
install (FILES ${INSTALL_HEADERS} DESTINATION include)
