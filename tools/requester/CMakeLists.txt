set (SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfRequester.c"
    )

add_executable (sbf-requester ${SOURCES})
target_link_libraries (sbf-requester sbfcore sbfcommon sbfrequestreply sbftransport)

install (TARGETS sbf-requester
    RUNTIME DESTINATION bin)
