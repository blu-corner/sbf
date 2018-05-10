set (INSTALL_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/SbfCxxTcpConnection.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/SbfCxxUdpConnection.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/SbfCxxTimer.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/SbfCxxQueue.h")

install (FILES ${INSTALL_HEADERS} DESTINATION include/sbf/cpp)
