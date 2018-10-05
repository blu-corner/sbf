set (INSTALL_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfEvent.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfHiResTimer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfMw.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfQueue.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTimer.h"
    )

set (SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfEvent.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfHiResTimer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfMw.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfQueue.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTimer.c"
    )

set (LIBS sbfcommon
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

add_library (sbfcore SHARED ${SOURCES})
target_link_libraries (sbfcore ${LIBS})

install (TARGETS sbfcore
         EXPORT sbfcore
         RUNTIME DESTINATION lib
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
install (FILES ${INSTALL_HEADERS} DESTINATION include/sbf)
