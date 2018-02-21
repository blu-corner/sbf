add_executable (tcpConnector tcpConnector.c)
target_link_libraries (tcpConnector sbftcpconnection sbfcommon sbftransport sbfmw)

install (TARGETS tcpConnector
    RUNTIME DESTINATION bin)
