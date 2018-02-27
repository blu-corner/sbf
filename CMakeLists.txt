cmake_minimum_required(VERSION 2.8.0)
project(sbf C)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(SUBMODULE_FLAGS -DTESTS=off)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
endif()

option(DEBUG "Enable debug build" OFF)

# allow external projects
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

# deps
find_library(EVENT_LIB
  NAMES event
  HINTS "${CMAKE_INSTALL_PREFIX}/lib"
  )
find_library(EVENT_PTHREAD_LIB
  NAMES event_pthreads
  HINTS "${CMAKE_INSTALL_PREFIX}/lib"
  )
if (EVENT_LIB AND EVENT_PTHREAD_LIB)
  message(STATUS "event-found: " ${EVENT_LIB})
  message(STATUS "event-pthread-found: " ${EVENT_PTHREAD_LIB})
elseif (NOT event_FOUND)
  set(LIBEVENT_CMAKE ${PROJECT_SOURCE_DIR}/thirdparty/libevent CACHE PATH "Location of libevent-cmake" FORCE)
  message(STATUS "libevent-location: " ${LIBEVENT_CMAKE})
  ExternalProject_Add(
    libevent
    SOURCE_DIR ${LIBEVENT_CMAKE}
    CMAKE_ARGS -DEVENT__DISABLE_TESTS=ON -DEVENT__DISABLE_REGRESS=ON -DEVENT__DISABLE_OPENSSL=ON -DEVENT__DISABLE_SAMPLES=ON -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    )
endif()

# add include and lib dir
link_directories(${CMAKE_INSTALL_PREFIX}/lib)
include_directories(${CMAKE_INSTALL_PREFIX}/include)

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
# compile options
add_compile_options(
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
if (DEBUG)
  message("sbf DEBUG: ON")
  add_compile_options(-g3)
  set(SUBMODULE_FLAGS ${SUBMODULE_FLAGS} -DDEBUG=on)
else ()
  message("sbf DEBUG: OFF")
  add_compile_options(-g -O2)
endif (DEBUG)

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

install(EXPORT sbfcore DESTINATION lib/sbf/cmake)
install(EXPORT sbfcommon DESTINATION lib/sbf/cmake)
install(EXPORT sbfcommonhandler DESTINATION lib/sbf/cmake)
install(EXPORT sbftcpmeshhandler DESTINATION lib/sbf/cmake)
install(EXPORT sbfudphandler DESTINATION lib/sbf/cmake)
install(EXPORT sbftransport DESTINATION lib/sbf/cmake)
install(EXPORT sbfrequestreply DESTINATION lib/sbf/cmake)

set(${PROJECT_NAME}_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/sbf)
set(${PROJECT_NAME}_LIBRARIES sbfcore sbfcommon sbfcommonhandler sbftcpmeshhandler sbfudphandler sbftransport sbfrequestreply)
configure_file(
  ${PROJECT_NAME}.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.cmake
  @ONLY
  )
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.cmake
  DESTINATION lib/cmake/sbf COMPONENT dev
  RENAME ${PROJECT_NAME}Config.cmake
  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
  )
