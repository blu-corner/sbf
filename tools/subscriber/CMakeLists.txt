set (SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfSubscriber.c"
    )

add_executable (sbf-subscriber ${SOURCES})
target_link_libraries (sbf-subscriber sbfcore sbfcommon sbftransport)

install (TARGETS sbf-subscriber
    RUNTIME DESTINATION bin)
