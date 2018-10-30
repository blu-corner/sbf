set (INSTALL_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTcpConnection.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTcpListener.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfUdpMulticast.h")

set (SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTcpConnection.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTcpListener.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfUdpMulticast.c")

set (LIBS sbfcommon
		  sbfcore
		  event
		  ${CMAKE_THREAD_LIBS_INIT}
		  ${CMAKE_DL_LIBS})
if(WIN32)
	set (LIBS "${LIBS}"
			  event_core
			  event_extra)
else()
	set (LIBS "${LIBS}"
			  event_pthreads)
endif()

add_library (sbfnetwork SHARED ${SOURCES})
target_link_libraries (sbfnetwork ${LIBS})

if (NOT EVENT_LIB)
    add_dependencies (sbfnetwork libevent)
endif()


install (TARGETS sbfnetwork
         EXPORT sbfnetwork
         RUNTIME DESTINATION lib
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
install (FILES ${INSTALL_HEADERS} DESTINATION include/sbf)
