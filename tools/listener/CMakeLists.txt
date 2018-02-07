add_executable (inetListener inetListener.c)
add_executable (unixListener unixListener.c)
target_link_libraries (inetListener sbfcore sbfcommon sbftransport)
target_link_libraries (unixListener sbfcore sbfcommon sbftransport)

install (TARGETS inetListener unixListener
    RUNTIME DESTINATION bin)
