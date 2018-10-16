if(WIN32)
	set (SOURCES
    	"${CMAKE_CURRENT_SOURCE_DIR}/sbfRequester.c"
    	"${CMAKE_SOURCE_DIR}/src/thirdparty/getopt/getopt.c")
else()
	set (SOURCES
    	"${CMAKE_CURRENT_SOURCE_DIR}/sbfRequester.c")
endif()

add_executable (sbf-requester ${SOURCES})
target_link_libraries (sbf-requester sbfcore sbfcommon sbfrequestreply sbftransport)

install (TARGETS sbf-requester
    RUNTIME DESTINATION bin)
