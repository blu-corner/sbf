add_executable (tcpConnector tcpConnector.c)
target_link_libraries (tcpConnector sbfcommon sbfcore sbfnetwork)

install (TARGETS tcpConnector
    RUNTIME DESTINATION bin)
