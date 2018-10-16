set (INSTALL_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfBatch.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfPub.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfSub.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTport.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTopic.h"
    )

set (SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfBatch.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfHandler.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfPub.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfSub.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTport.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTportFragments.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTportWeight.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sbfTopic.c"
    )

set (LIBS sbfcommon
		  sbfcore)
if(WIN32)
	set (LIBS "${LIBS}"
			  pcre3)
endif()

add_library (sbftransport SHARED ${SOURCES})
target_link_libraries (sbftransport ${LIBS})

install (TARGETS sbftransport
         EXPORT sbftransport
         RUNTIME DESTINATION lib
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
install (FILES ${INSTALL_HEADERS} DESTINATION include/sbf)
