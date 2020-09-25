if(WIN32)
    set (SOURCES
         "${CMAKE_CURRENT_SOURCE_DIR}/sbfPublisher.c"
         "${CMAKE_SOURCE_DIR}/src/thirdparty/getopt/getopt.c")
else()
    set (SOURCES
         "${CMAKE_CURRENT_SOURCE_DIR}/sbfPublisher.c")
    set (CPP_SOURCES
         "${CMAKE_CURRENT_SOURCE_DIR}/sbfPublisher.cpp")
endif()

add_executable (sbf-publisher ${SOURCES})
target_link_libraries (sbf-publisher sbfcore sbfcommon sbftransport)

add_executable (sbf-cpp-publisher ${CPP_SOURCES})
target_link_libraries (sbf-cpp-publisher sbfcore sbfcommon sbftransport)

install (TARGETS sbf-publisher sbf-cpp-publisher
    RUNTIME DESTINATION bin)
