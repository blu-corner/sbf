if(WIN32)
    set (SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/sbfSubscriber.c"
        "${CMAKE_SOURCE_DIR}/src/thirdparty/getopt/getopt.c")
else()
    set (SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/sbfSubscriber.c")
    set (CPP_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/sbfSubscriber.cpp")
endif()

add_executable (sbf-subscriber ${SOURCES})
target_link_libraries (sbf-subscriber sbfcore sbfcommon sbftransport)

add_executable (sbf-cpp-subscriber ${CPP_SOURCES})
target_link_libraries (sbf-cpp-subscriber sbfcpp
                                          sbfcore
                                          sbfcommon
                                          sbftransport)

install (TARGETS sbf-subscriber sbf-cpp-subscriber
    RUNTIME DESTINATION bin)
