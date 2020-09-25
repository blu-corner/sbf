if(WIN32)
	set (SOURCES
    	"${CMAKE_CURRENT_SOURCE_DIR}/sbfRequester.c"
    	"${CMAKE_SOURCE_DIR}/src/thirdparty/getopt/getopt.c")
else()
	set (SOURCES
    	"${CMAKE_CURRENT_SOURCE_DIR}/sbfRequester.c")
	set (CPP_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/sbfRequester.cpp")
endif()

add_executable (sbf-requester ${SOURCES})
target_link_libraries (sbf-requester sbfcore sbfcommon sbfrequestreply sbftransport)

add_executable (sbf-cpp-requester ${CPP_SOURCES})
target_link_libraries (sbf-cpp-requester sbfcore sbfcommon sbfrequestreply sbftransport)

install (TARGETS sbf-requester sbf-cpp-requester
    RUNTIME DESTINATION bin)
