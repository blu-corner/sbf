cmake_minimum_required(VERSION 3.0.0)
project(SBF C)

if (NOT ${CMAKE_INSTALL_PREFIX})
  set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
endif ()

# allow external projects
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

# deps
ExternalProject_Add(
  libevent
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/libevent
  CMAKE_ARGS -DEVENT__DISABLE_TESTS=ON -DEVENT__DISABLE_REGRESS=ON -DEVENT__DISABLE_OPENSSL=ON -DEVENT__DISABLE_SAMPLES=ON -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
  )

# add include and lib dir
link_directories(${CMAKE_BINARY_DIR}/install/lib)
include_directories(${CMAKE_BINARY_DIR}/libevent/include)

# set version info
set(SBF_MAJOR_VERSION 1)
set(SBF_MINOR_VERSION 0)
set(SBF_PATCH_VERSION 0)
set(SBF_VERSION ${SBF_MAJOR_VERSION}.${SBF_MINOR_VERSION}.${SBF_PATCH_VERSION})

if (UNIX)
  macro (add_library _name)
    _add_library(${ARGV})
    if (TARGET ${_name})
      SET_TARGET_PROPERTIES(${_name}
        PROPERTIES
            VERSION ${SBF_VERSION}
            # SOVERSION 0.0.0
        )
    endif()
  endmacro()
endif (UNIX)
        

set(CMAKE_C_STANDARD 99)
add_compile_options(
  -g
  -O2
  -Wall
  -Wextra
  -Wbad-function-cast
  -Wcast-align
  -Wfloat-equal
  -Wformat=2
  -Wno-format-y2k
  -Wno-missing-field-initializers
  -Wno-unused-parameter
  -Wpointer-arith
  -Wsign-compare
  -Wwrite-strings
  )

include_directories(
  ${PROJECT_SOURCE_DIR}/core
  ${PROJECT_SOURCE_DIR}/common
  ${PROJECT_SOURCE_DIR}/requestreply
  ${PROJECT_SOURCE_DIR}/transport
  ${PROJECT_SOURCE_DIR}/handlers/common
  )

add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(transport)
add_subdirectory(requestreply)
add_subdirectory(handlers)
add_subdirectory(tools)
