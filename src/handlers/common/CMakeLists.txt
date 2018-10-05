set (SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfCommonHandler.c"
    )

add_library (sbfcommonhandler SHARED ${SOURCES})
target_link_libraries (sbfcommonhandler sbfcommon sbfcore sbftransport sbfnetwork)

install (TARGETS sbfcommonhandler
         EXPORT sbfcommonhandler
         RUNTIME DESTINATION lib
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
