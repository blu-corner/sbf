if(WIN32)
	set (SOURCES
    	"${CMAKE_CURRENT_SOURCE_DIR}/sbfPublisher.c"
    	"${CMAKE_SOURCE_DIR}/src/thirdparty/getopt/getopt.c")
else()
	set (SOURCES
    	"${CMAKE_CURRENT_SOURCE_DIR}/sbfPublisher.c")
endif()

add_executable (sbf-publisher ${SOURCES})
target_link_libraries (sbf-publisher sbfcore sbfcommon sbftransport)

install (TARGETS sbf-publisher
    RUNTIME DESTINATION bin)
