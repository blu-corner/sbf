set (INSTALL_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfRequestPub.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfRequestReply.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfRequestSub.h"
    )

set (SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfRequestPub.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfRequestReply.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfRequestSub.c"
    )

add_library (sbfrequestreply SHARED ${SOURCES})

install (TARGETS sbfrequestreply
         LIBRARY DESTINATION lib)
install (FILES ${INSTALL_HEADERS} DESTINATION include)
