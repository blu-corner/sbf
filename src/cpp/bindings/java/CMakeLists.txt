#
# Copyright 2014-2020 Neueda Ltd.
#
# Include Java
find_package(Java REQUIRED COMPONENTS Runtime Development)
find_package(JNI REQUIRED)
include(UseJava)

include_directories(${JNI_INCLUDE_DIRS} ../)

# set the flags for swig
set(CMAKE_SWIG_FLAGS -package com.neueda.sbf -v)
set_source_files_properties(../sbf.java.i PROPERTIES CPLUSPLUS ON)

# Add swig module
add_compile_options(-fno-strict-aliasing)
swig_add_module(Sbf java ../sbf.java.i)
swig_link_libraries(Sbf sbfcpp
                        sbfcommon
                        sbfcore
                        sbftransport
                        sbfnetwork
                        sbfrequestreply
                        ${JNI_LIBRARIES})

set(CMAKE_JAR_CLASSES_PREFIX com/neueda/sbf)
set(JNI_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/SWIGTYPE_p_p_void.java
    ${CMAKE_CURRENT_BINARY_DIR}/SWIGTYPE_p_sbfBuffer.java
    ${CMAKE_CURRENT_BINARY_DIR}/SWIGTYPE_p_sbfKeyValue.java
    ${CMAKE_CURRENT_BINARY_DIR}/SWIGTYPE_p_sbfLog.java
    ${CMAKE_CURRENT_BINARY_DIR}/SWIGTYPE_p_sbfMw.java
    ${CMAKE_CURRENT_BINARY_DIR}/SWIGTYPE_p_sbfMwThread.java
    ${CMAKE_CURRENT_BINARY_DIR}/SWIGTYPE_p_sbfPub.java
    ${CMAKE_CURRENT_BINARY_DIR}/SWIGTYPE_p_sbfQueue.java
    ${CMAKE_CURRENT_BINARY_DIR}/SWIGTYPE_p_sbfRequestImpl.java
    ${CMAKE_CURRENT_BINARY_DIR}/SWIGTYPE_p_sbfRequestPub.java
    ${CMAKE_CURRENT_BINARY_DIR}/SWIGTYPE_p_sbfRequestSub.java
    ${CMAKE_CURRENT_BINARY_DIR}/SWIGTYPE_p_sbfSub.java
    ${CMAKE_CURRENT_BINARY_DIR}/SWIGTYPE_p_sbfTopicImpl.java
    ${CMAKE_CURRENT_BINARY_DIR}/SWIGTYPE_p_sbfTimer.java
    ${CMAKE_CURRENT_BINARY_DIR}/SWIGTYPE_p_sbfTport.java
    ${CMAKE_CURRENT_BINARY_DIR}/SWIGTYPE_p_u_int.java
    ${CMAKE_CURRENT_BINARY_DIR}/SWIGTYPE_p_void.java
    ${CMAKE_CURRENT_BINARY_DIR}/SbfBuffer.java
    ${CMAKE_CURRENT_BINARY_DIR}/SbfKeyValue.java
    ${CMAKE_CURRENT_BINARY_DIR}/SbfLog.java
    ${CMAKE_CURRENT_BINARY_DIR}/SbfMw.java
    ${CMAKE_CURRENT_BINARY_DIR}/SbfPub.java
    ${CMAKE_CURRENT_BINARY_DIR}/SbfPubDelegate.java
    ${CMAKE_CURRENT_BINARY_DIR}/SbfQueue.java
    ${CMAKE_CURRENT_BINARY_DIR}/SbfQueueDelegate.java
    ${CMAKE_CURRENT_BINARY_DIR}/SbfRequestPub.java
    ${CMAKE_CURRENT_BINARY_DIR}/SbfRequestPubDelegate.java
    ${CMAKE_CURRENT_BINARY_DIR}/SbfRequestSub.java
    ${CMAKE_CURRENT_BINARY_DIR}/SbfRequestSubDelegate.java
    ${CMAKE_CURRENT_BINARY_DIR}/SbfSub.java
    ${CMAKE_CURRENT_BINARY_DIR}/SbfSubDelegate.java
    ${CMAKE_CURRENT_BINARY_DIR}/SbfTimer.java
    ${CMAKE_CURRENT_BINARY_DIR}/SbfTimerDelegate.java
    ${CMAKE_CURRENT_BINARY_DIR}/SbfTport.java
    ${CMAKE_CURRENT_BINARY_DIR}/sbf.java
    ${CMAKE_CURRENT_BINARY_DIR}/sbfConstants.java
    ${CMAKE_CURRENT_BINARY_DIR}/sbfJNI.java
    ${CMAKE_CURRENT_BINARY_DIR}/sbfLogLevel.java
  )

add_jar(SbfJNI
  ${JNI_SOURCES}
  )

if(TARGET _Sbf)
  add_dependencies(SbfJNI _Sbf)
else()
  add_dependencies(SbfJNI Sbf)
endif()

# Files to install with Java
if(APPLE)
  set(JAVA_DYLIB_SUFFIX ".jnilib")
else()
  set(JAVA_DYLIB_SUFFIX ".so")
endif()

# cmake 2.8 swig has issues on figuring out naming libs
if(TARGET _Sbf)
  set_target_properties(_Sbf PROPERTIES OUTPUT_NAME "Sbf")
  set_target_properties(_Sbf PROPERTIES SUFFIX ${JAVA_DYLIB_SUFFIX})
endif()

set(JAVA_INSTALL_FILES
  ${CMAKE_CURRENT_BINARY_DIR}/SbfJNI.jar)

set(JAVA_EXEC_INSTALL_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/build.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/run.sh)

# Declare install target for java
install(FILES ${JAVA_INSTALL_FILES}
  DESTINATION lib/java)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples
  DESTINATION examples/java)
install(FILES ${JAVA_EXEC_INSTALL_FILES}
  PERMISSIONS OWNER_EXECUTE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
  DESTINATION examples/java)

if(TARGET _Sbf)
  install(TARGETS _Sbf
	DESTINATION lib/java)
else()
  install(TARGETS Sbf
	DESTINATION lib/java)
endif()