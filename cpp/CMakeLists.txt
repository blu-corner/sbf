set (INSTALL_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/SbfCxxTcpConnection.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/SbfCxxQueue.h")

set (SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/SbfCxxTcpConnection.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/SbfCxxQueue.cpp")

add_library (sbfcxx SHARED ${SOURCES})
target_link_libraries (sbfcxx sbfcommon sbfcore)

install (TARGETS sbfcxx
         EXPORT sbfcxx
         COMPONENT library
         RUNTIME DESTINATION lib
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
install (FILES ${INSTALL_HEADERS} DESTINATION include/sbf/cpp)
