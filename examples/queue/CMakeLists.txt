if(WIN32)
	set (SOURCES
    	"${CMAKE_CURRENT_SOURCE_DIR}/sbfQueue.c"
    	"${CMAKE_SOURCE_DIR}/thirdparty/getopt/getopt.c"
    	"${CMAKE_SOURCE_DIR}/thirdparty/getopt/getopt_long.c")
else()
	set (SOURCES
    	"${CMAKE_CURRENT_SOURCE_DIR}/sbfQueue.c")
endif()

add_executable (sbf-queue ${SOURCES})
target_link_libraries (sbf-queue sbfcore sbfcommon)

install (TARGETS sbf-queue
    RUNTIME DESTINATION bin)
