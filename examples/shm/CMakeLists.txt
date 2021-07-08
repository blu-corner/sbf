add_executable (shmServer shmServer.c)
target_link_libraries (shmServer sbfcommon)

add_executable (shmClient shmClient.c)
target_link_libraries (shmClient sbfcommon)

add_executable (shmBlockingClient shmBlockingClient.c)
target_link_libraries (shmBlockingClient sbfcommon)

add_executable (shmBlockingServer shmBlockingServer.c)
target_link_libraries (shmBlockingServer sbfcommon)

install (TARGETS shmServer shmClient shmBlockingServer shmBlockingClient RUNTIME DESTINATION bin)
