cmake_minimum_required(VERSION 2.8)
project(FooBar C)

set(SBF_MAJOR_VERSION 1)
set(SBF_MINOR_VERSION 0)
set(SBF_PATCH_VERSION 0)

set(SBF_VERSION
  ${SBF_MAJOR_VERSION}.${SBF_MINOR_VERSION}.${SBF_PATCH_VERSION})

set(CMAKE_INSTALL_PREFIX "install")

include_directories(
    "./core"
    "./common"
    "./properties"
    "./requestreply"
    "./transport"
    "./handlers/common"
)

if (UNIX)
    include_directories(
        "./thirdparty/libevent/linux/include"
    )

    link_directories(
        "${CMAKE_SOURCE_DIR}/thirdparty/libevent/linux/lib"
    )

    find_library(
        COMMON_LIBS
        dl
        event
        event_pthreads
        rt
        PATHS
        ./thirdparty/libevent/linux/lib/
    )

    set (LINKER_FLAGS "-pthread -Wl,--no-as-needed")
    set (CMAKE_SHARED_LINKER_FLAGS ${LINKER_FLAGS})
    set (CMAKE_EXE_LINKER_FLAGS ${LINKER_FLAGS})

    set (CMAKE_C_FLAGS
        "-std=gnu99 -O2 -pthread -Wall -Wextra -Wbad-function-cast -Wcast-align -Wfloat-equal -Wformat=2 -Wno-format-y2k -Wno-missing-field-initializers -Wno-unused-parameter -Wpointer-arith -Wsign-compare -Wwrite-strings") 
endif (UNIX)

add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(transport)
add_subdirectory(requestreply)
add_subdirectory(handlers)
add_subdirectory(tools)
