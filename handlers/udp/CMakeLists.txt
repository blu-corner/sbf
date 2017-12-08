set (SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfUdpHandler.c"
    )

add_library (sbfudphandler SHARED ${SOURCES})
target_link_libraries (sbfudphandler sbfcommon sbfcommonhandler sbfcore sbftransport)

install (TARGETS sbfudphandler EXPORT sbfudphandler
         LIBRARY DESTINATION lib)
