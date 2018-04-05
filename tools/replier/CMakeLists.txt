if(WIN32)
    set (SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/sbfReplier.c"
    	"${CMAKE_SOURCE_DIR}/thirdparty/getopt/getopt.c"
    	"${CMAKE_SOURCE_DIR}/thirdparty/getopt/getopt_long.c")
else()
	set (SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/sbfReplier.c"
        )
endif()

add_executable (sbf-replier ${SOURCES})
target_link_libraries (sbf-replier sbfcommon sbfcore sbfrequestreply sbftransport)

install (TARGETS sbf-replier
    RUNTIME DESTINATION bin)
