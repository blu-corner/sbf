FILE(GLOB INSTALL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

set(LIBS sbfcommon
         sbfcore
         sbfrequestreply
         sbftransport
         ${CMAKE_THREAD_LIBS_INIT}
         ${CMAKE_DL_LIBS}
         ${EVENT_LIB})

if(WIN32)
  set (LIBS "${LIBS}"
    ${EVENT_CORE_LIB}
    ${EVENT_EXTRA_LIB})
else()
  set (LIBS "${LIBS}"
    ${EVENT_PTHREAD_LIB})
endif()

add_library (sbfcpp SHARED ${SOURCES})
target_link_libraries (sbfcpp ${LIBS})

if (NOT EVENT_LIB)
    add_dependencies (sbfcpp libevent)
endif()

install (TARGETS sbfcpp
         EXPORT sbfcpp
         RUNTIME DESTINATION lib
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
install (FILES ${INSTALL_HEADERS} DESTINATION include/sbf/cpp)

add_subdirectory(bindings)
