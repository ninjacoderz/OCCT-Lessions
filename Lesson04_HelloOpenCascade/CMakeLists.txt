cmake_minimum_required (VERSION 4.0)

# Project name
project (Lesson_04_OCCT)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../adm/cmake" ${CMAKE_MODULE_PATH})
# OpenCascade
find_package(OpenCASCADE REQUIRED)

if (NOT OpenCASCADE_FOUND)
  message (FATAL_ERROR "could not find OpenCASCADE, please set OpenCASCADE_DIR variable" )
else()
  message (STATUS "Using OpenCASCADE from \"${OpenCASCADE_INSTALL_PREFIX}\"" )
  message (STATUS "OpenCASCADE_INCLUDE_DIR=${OpenCASCADE_INCLUDE_DIR}")
  message (STATUS "OpenCASCADE_LIBRARY_DIR=${OpenCASCADE_LIBRARY_DIR}")
  # Configure C++ compiler's includes dir
  INCLUDE_DIRECTORIES ( SYSTEM ${OpenCASCADE_INCLUDE_DIR} )
endif()

# glfw
find_package(glfw3 REQUIRED PATHS "/Users/binhnguyenthanh/Documents/OCCT/glfw/build/lib/cmake/glfw3")
if (glfw3_FOUND)
  message (STATUS "Using glfw3 ${glfw3_VERSION}" )
  message (STATUS "GLFW_LIBRARY_DIRS ${GLFW_LIBRARY_DIRS}" )
  INCLUDE_DIRECTORIES(${GLFW_INCLUDE_DIRS})
  LINK_DIRECTORIES(${GLFW_LIBRARY_DIRS})
else()
  message (STATUS "glfw3 is not found." )
endif()

message(STATUS "GLFW3 include dir: ${GLFW_INCLUDE_DIRS}")
message(STATUS "GLFW3 library dir: ${GLFW_LIBRARY_DIRS}")

file(GLOB SOURCES
  *.h
  *.cpp
)
# Add executable
add_executable (Lesson_04
   ${SOURCES}
)

SET(OpenCASCADE_LIBS 
  TKernel
  TKMath
  TKBRep
  TKGeomBase
  TKG2d
  TKG3d
  TKGeomAlgo
  TKPrim
  TKBool
  TKBO       
  TKTopAlgo
  TKFillet
  TKOffset
  TKOpenGl
  TKV3d
  TKService
  TKShHealing
  TKCAF
)

target_link_libraries (
  Lesson_04
  ${OpenCASCADE_LIBS}
  glfw
)
