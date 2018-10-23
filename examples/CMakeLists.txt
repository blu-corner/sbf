add_subdirectory (replier)
add_subdirectory (requester)
add_subdirectory (publisher)

if(NOT WIN32)
	add_subdirectory (connector)
endif()
add_subdirectory (subscriber)
add_subdirectory (queue)
