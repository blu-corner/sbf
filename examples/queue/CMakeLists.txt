if(WIN32)
	set (SOURCES
    	"${CMAKE_CURRENT_SOURCE_DIR}/sbfQueue.c"
    	"${CMAKE_SOURCE_DIR}/src/thirdparty/getopt/getopt.c")
else()
	set (SOURCES
    	"${CMAKE_CURRENT_SOURCE_DIR}/sbfQueue.c")
	set (CPP_SOURCES
         "${CMAKE_CURRENT_SOURCE_DIR}/sbfQueue.cpp")
endif()

add_executable (sbf-queue ${SOURCES})
target_link_libraries (sbf-queue sbfcore sbfcommon)

add_executable (sbf-cpp-queue ${CPP_SOURCES})
target_link_libraries (sbf-cpp-queue sbfcore sbfcommon)

install (TARGETS sbf-queue sbf-cpp-queue
    RUNTIME DESTINATION bin)
