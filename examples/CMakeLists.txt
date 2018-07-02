add_subdirectory (replier)
add_subdirectory (requester)

if(NOT WIN32)
	add_subdirectory (publisher)
endif()
add_subdirectory (subscriber)
add_subdirectory (connector)
add_subdirectory (queue)
