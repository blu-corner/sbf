set (SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfReplier.c"
    )

add_executable (sbf-replier ${SOURCES})
target_link_libraries (sbf-replier sbfcore sbfcommon sbfrequestreply sbftransport)

install (TARGETS sbf-replier
    RUNTIME DESTINATION bin)
