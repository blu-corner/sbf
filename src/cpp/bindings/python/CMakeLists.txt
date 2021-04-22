#
# Copyright 2014-2018 Neueda Ltd.
#
# Include python

# set the flags for swig
set(CMAKE_SWIG_FLAGS -module sbf -threads -v)
set_source_files_properties(../sbf.py.i PROPERTIES CPLUSPLUS ON)

execute_process(
    COMMAND bash -c "${PYTHON_CONFIG} --includes"
    OUTPUT_VARIABLE PYTHON_INCLUDE_DIRS
    RESULT_VARIABLE RET
    )
if(NOT ${RET} EQUAL "0")
    message(FATAL_ERROR "failed to run python-config --includes")
endif()

execute_process(
    COMMAND bash -c "${PYTHON_CONFIG} --ldflags"
    OUTPUT_VARIABLE PYTHON_LDFLAGS
    RESULT_VARIABLE RET
    )
if(NOT ${RET} EQUAL "0")
    message(FATAL_ERROR "failed to run python-config --ldflags")
endif()

execute_process(
    COMMAND bash -c "${PYTHON_CONFIG} --libs"
    OUTPUT_VARIABLE PYTHON_LIBS
    RESULT_VARIABLE RET
    )
if(NOT ${RET} EQUAL "0")
    message(FATAL_ERROR "failed to run python-config --libs")
endif()

string(STRIP ${PYTHON_LDFLAGS} PYTHON_LDFLAGS)
string(STRIP ${PYTHON_INCLUDE_DIRS} PYTHON_INCLUDE_DIRS)
string(STRIP ${PYTHON_LIBS} PYTHON_LIBS)

message(STATUS "Python includes: ${PYTHON_INCLUDE_DIRS}")
message(STATUS "Python ldflags: ${PYTHON_LDFLAGS}")
message(STATUS "Python libs: ${PYTHON_LIBS}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PYTHON_INCLUDE_DIRS}")
link_libraries(${PYTHON_LDFLAGS})

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src/cpp/bindings
  ${CMAKE_INSTALL_PREFIX}/include
  ${CMAKE_INSTALL_PREFIX}/include/sbf
  )

# Add swig module
swig_add_module(PySbf python ../sbf.py.i)
swig_link_libraries(PySbf
                    sbfcpp
                    sbfcommon
                    sbfcore
                    sbftransport
                    sbfnetwork
                    sbfrequestreply
                    ${PYTHON_LIBS})
set_target_properties(_PySbf PROPERTIES OUTPUT_NAME "_sbf")

# Files to install with Python
set(PYTHON_INSTALL_FILES
  ${CMAKE_CURRENT_BINARY_DIR}/sbf.py)

# Declare install target for pytho
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples
  DESTINATION examples/python)
install(TARGETS _PySbf
  DESTINATION lib/python)
install(FILES ${PYTHON_INSTALL_FILES}
  DESTINATION lib/python)