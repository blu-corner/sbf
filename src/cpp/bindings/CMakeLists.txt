#
# Copyright 2014-2020 Neueda Ltd.
#
if(JAVA OR PYTHON)
  find_package(SWIG REQUIRED)
  include(${SWIG_USE_FILE})
endif()

if(JAVA)
  add_subdirectory(java)
endif()

if(PYTHON)
  add_subdirectory(python)
endif()