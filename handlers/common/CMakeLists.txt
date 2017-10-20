set (SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfCommonHandler.c"
    )

add_library (sbfcommonhandler SHARED ${SOURCES})
target_link_libraries (sbfcommonhandler sbfcommon sbfcore sbftransport)

install (TARGETS sbfcommonhandler
         LIBRARY DESTINATION lib)
