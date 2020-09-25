if(WIN32)
    set (SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/sbfReplier.c"
    	"${CMAKE_SOURCE_DIR}/src/thirdparty/getopt/getopt.c")
else()
	set (SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/sbfReplier.c"
        )
	set (CPP_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/sbfReplier.cpp"
        )
endif()

add_executable (sbf-replier ${SOURCES})
target_link_libraries (sbf-replier sbfcommon sbfcore sbfrequestreply sbftransport)

add_executable (sbf-cpp-replier ${CPP_SOURCES})
target_link_libraries (sbf-cpp-replier sbfcommon sbfcore sbfrequestreply sbftransport)

install (TARGETS sbf-replier sbf-cpp-replier
    RUNTIME DESTINATION bin)
