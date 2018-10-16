if(WIN32)
	set (SOURCES
    	"${CMAKE_CURRENT_SOURCE_DIR}/sbfSubscriber.c"
    	"${CMAKE_SOURCE_DIR}/src/thirdparty/getopt/getopt.c")
else()
	set (SOURCES
    	"${CMAKE_CURRENT_SOURCE_DIR}/sbfSubscriber.c")
endif()

add_executable (sbf-subscriber ${SOURCES})
target_link_libraries (sbf-subscriber sbfcore sbfcommon sbftransport)

install (TARGETS sbf-subscriber
    RUNTIME DESTINATION bin)
