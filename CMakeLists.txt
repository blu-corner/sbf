cmake_minimum_required(VERSION 2.8.0)
project(sbf C CXX)
set(SUBMODULE_FLAGS -DTESTS=off)

if(PYTHON_LIBRARY AND PYTHON_INCLUDE_DIR AND PYTHON_EXECUTABLE)
    set(SUBMODULE_FLAGS ${SUBMODULE_FLAGS} -DPYTHON_LIBRARY=${PYTHON_LIBRARY})
    set(SUBMODULE_FLAGS ${SUBMODULE_FLAGS} -DPYTHON_INCLUDE_DIR=${PYTHON_INCLUDE_DIR})
    set(SUBMODULE_FLAGS ${SUBMODULE_FLAGS} -DPYTHON_EXECUTABLE=${PYTHON_EXECUTABLE})
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
endif()

if(WIN32)
  set(CMAKE_SHARED_LINKER_FLAGS "/FORCE:UNRESOLVED")
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
  set(BUILD_SHARED_LIBS TRUE)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
  link_directories(${CMAKE_SOURCE_DIR}/src/thirdparty/pcre/x64/lib)
  include_directories(${CMAKE_SOURCE_DIR}/src/thirdparty/getopt)
  include_directories(${CMAKE_SOURCE_DIR}/src/thirdparty/pcre/x64/include)

  file(GLOB THIRDPARTY_LIB ${CMAKE_SOURCE_DIR}/src/thirdparty/pcre/x64/lib/*)
  set(THIRDPARTY_INCLUDES ${CMAKE_SOURCE_DIR}/src/thirdparty/getopt/getopt.h
						  ${CMAKE_SOURCE_DIR}/src/thirdparty/pcre/x64/include/pcre.h
						  ${CMAKE_SOURCE_DIR}/src/thirdparty/pcre/x64/include/pcreposix.h
						  ${CMAKE_SOURCE_DIR}/src/thirdparty/pcre/x64/include/regex.h)

  INSTALL(FILES ${THIRDPARTY_INCLUDES}
          DESTINATION include/sbf)
  INSTALL(FILES ${THIRDPARTY_LIB}
          DESTINATION lib)
endif()

if(APPLE)
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
  set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-undefined,dynamic_lookup")
  set(CMAKE_MACOSX_RPATH 1)
endif()

option(DEBUG "Enable debug build" OFF)
option(COVERAGE "Enable gcov coverage" OFF)
option(EXAMPLES "Enable examples" ON)
option(JAVA "Enable Java bindings" OFF)
option(PYTHON "Enable Python bindings" OFF)
set(PYTHON_CONFIG "python-config" CACHE STRING "python-config for build env config")

# allow external projects
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

# deps
if(NOT SUBMODULE_ROOT_DIR)
    set(SUBMODULE_ROOT_DIR ${PROJECT_SOURCE_DIR})
endif()
if(NOT SUBMODULE_PREFIX_DIR)
    set(SUBMODULE_PREFIX_DIR ${CMAKE_BINARY_DIR})
endif()

find_package(Threads)
find_library(EVENT_LIB
  NAMES event
  HINTS "${CMAKE_INSTALL_PREFIX}/lib")
find_library(EVENT_PTHREAD_LIB
  NAMES event_pthreads
  HINTS "${CMAKE_INSTALL_PREFIX}/lib")
find_library(EVENT_CORE_LIB
  NAMES event_core
  HINTS "${CMAKE_INSTALL_PREFIX}/lib")
find_library(EVENT_EXTRA_LIB
  NAMES event_extra
  HINTS "${CMAKE_INSTALL_PREFIX}/lib")
find_path(EVENT_INCLUDE_DIR event2/event.h
  PATHS "${CMAKE_INSTALL_PREFIX}/include")

if (EVENT_LIB AND EVENT_PTHREAD_LIB)
  message(STATUS "event-found: " ${EVENT_LIB})
  message(STATUS "event-core-found: " ${EVENT_CORE_LIB})
  message(STATUS "event-pthread-found: " ${EVENT_PTHREAD_LIB})
  message(STATUS "event-headers-found: " ${EVENT_INCLUDE_DIR})
  include_directories(${EVENT_INCLUDE_DIR})
elseif (NOT event_FOUND)
  set(LIBEVENT_CMAKE ${SUBMODULE_ROOT_DIR}/ext/libevent CACHE PATH "Location of libevent-cmake" FORCE)
  message(STATUS "libevent-location: " ${LIBEVENT_CMAKE})
  ExternalProject_Add(
    libevent
    PREFIX ${SUBMODULE_PREFIX_DIR}
    SOURCE_DIR ${LIBEVENT_CMAKE}
    CMAKE_ARGS -DEVENT__DISABLE_DEBUG=ON -DEVENT__DISABLE_BENCHMARK=ON -DEVENT__DISABLE_TESTS=ON -DEVENT__DISABLE_REGRESS=ON -DEVENT__DISABLE_OPENSSL=ON -DEVENT__DISABLE_SAMPLES=ON -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    )
  set(EVENT_LIB event)
  set(EVENT_CORE_LIB event_core)
  set(EVENT_PTHREAD_LIB event_pthreads)
  set(EVENT_EXTRA_LIB event_extra)
endif()

# add include and lib dir
link_directories(${CMAKE_INSTALL_PREFIX}/lib)
include_directories(${CMAKE_INSTALL_PREFIX}/include
  ${CMAKE_INSTALL_PREFIX}/include/sbf)

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
if(UNIX)
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
endif()

if (DEBUG)
  message(STATUS "sbf DEBUG: ON")
  if(UNIX)
    add_compile_options(-g3)
  endif()
  set(SUBMODULE_FLAGS ${SUBMODULE_FLAGS} -DDEBUG=on)
else ()
  message(STATUS "sbf DEBUG: OFF")
  if(UNIX)
    add_compile_options(-g -O2)
  endif()
endif (DEBUG)
if (COVERAGE)
    set(COVERAGE_COMPILE_FLAGS "-g -O0 -coverage -fprofile-arcs -ftest-coverage")
    set(COVERAGE_LINK_FLAGS    "-coverage -lgcov")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${COVERAGE_COMPILE_FLAGS}" )
    set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${COVERAGE_LINK_FLAGS}" )
endif(COVERAGE)

include_directories(
  ${PROJECT_SOURCE_DIR}/src/core
  ${PROJECT_SOURCE_DIR}/src/common
  ${PROJECT_SOURCE_DIR}/src/network
  ${PROJECT_SOURCE_DIR}/src/requestreply
  ${PROJECT_SOURCE_DIR}/src/transport
  ${PROJECT_SOURCE_DIR}/src/handlers/common
  ${PROJECT_SOURCE_DIR}/src/cpp
  )

add_subdirectory(src)
if(EXAMPLES)
add_subdirectory(examples)
endif()

install(EXPORT sbfcore DESTINATION lib/cmake/sbf COMPONENT dev)
install(EXPORT sbfcommon DESTINATION lib/cmake/sbf COMPONENT dev)
install(EXPORT sbfcommonhandler DESTINATION lib/cmake/sbf COMPONENT dev)
install(EXPORT sbftcpmeshhandler DESTINATION lib/cmake/sbf COMPONENT dev)
install(EXPORT sbfudphandler DESTINATION lib/cmake/sbf COMPONENT dev)
install(EXPORT sbftransport  DESTINATION lib/cmake/sbf COMPONENT dev)
install(EXPORT sbfrequestreply DESTINATION lib/cmake/sbf COMPONENT dev)
install(EXPORT sbfnetwork DESTINATION lib/cmake/sbf COMPONENT dev)

set(${PROJECT_NAME}_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/sbf)
#set(${PROJECT_NAME}_LIBRARIES sbfcore sbfcommon sbfcommonhandler sbftcpmeshhandler sbfudphandler sbftransport sbfrequestreply)
set(${PROJECT_NAME}_LIBRARIES sbfcore sbfcommon)
configure_file(
  ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config-install.cmake
  @ONLY
  )
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config-install.cmake
  DESTINATION lib/cmake/${PROJECT_NAME} COMPONENT dev
  RENAME ${PROJECT_NAME}Config.cmake
  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
  )

set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_VERSION_MAJOR "${SBF_MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${SBF_MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "${SBF_PATCH_VERSION}")
set(CPACK_PACKAGE_NAME "sbf")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
if(DEBUG)
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_dbg-${CPACK_PACKAGE_VERSION}")
else()
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
endif()

INCLUDE(CPack)
