set (INSTALL_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/SbfTcpConnection.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/SbfUdpConnection.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/SbfTcpListener.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/SbfTimer.hpp")

install (FILES ${INSTALL_HEADERS} DESTINATION include/sbf/cpp)
