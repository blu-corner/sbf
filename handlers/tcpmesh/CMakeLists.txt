set (SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTcpMeshHandler.c"
    )

add_library (sbftcpmeshhandler SHARED ${SOURCES})
target_link_libraries (sbftcpmeshhandler sbfcommon sbfcore sbftransport)

install (TARGETS sbftcpmeshhandler
         LIBRARY DESTINATION lib)
