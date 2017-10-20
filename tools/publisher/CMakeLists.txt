set (SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfPublisher.c"
    )

add_executable (sbf-publisher ${SOURCES})
target_link_libraries (sbf-publisher sbfcore sbfcommon sbftransport)

install (TARGETS sbf-publisher
    RUNTIME DESTINATION bin)
