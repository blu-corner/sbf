add_executable (tcpConnector tcpConnector.c)
target_link_libraries (tcpConnector sbfcommon)

install (TARGETS tcpConnector
    RUNTIME DESTINATION bin)
