cmake_minimum_required (VERSION 4.0)

# Project name
project (Lesson_12_XDE CXX)

# Set module path

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../adm/cmake" ${CMAKE_MODULE_PATH})

# OpenCascade
find_package(OpenCASCADE REQUIRED)

if (NOT OpenCASCADE_FOUND)
  message (FATAL_ERROR "could not find OpenCASCADE, please set OpenCASCADE_DIR variable" )
else()
  message (STATUS "Using OpenCASCADE from \"${OpenCASCADE_INSTALL_PREFIX}\"" )
  message (STATUS "OpenCASCADE_INCLUDE_DIR=${OpenCASCADE_INCLUDE_DIR}")
  message (STATUS "OpenCASCADE_LIBRARY_DIR=${OpenCASCADE_LIBRARY_DIR}")
  message (STATUS "OpenCASCADE_LIBRARIES=${OpenCASCADE_LIBRARIES}")
  INCLUDE_DIRECTORIES(${OpenCASCADE_INCLUDE_DIR})
endif()


find_package(glfw3 REQUIRED PATHS "/Users/binhnguyenthanh/Documents/OCCT/glfw/build/lib/cmake/glfw3")
if (glfw3_FOUND)
  message (STATUS "Using glfw3 ${glfw3_VERSION}" )
  message (STATUS "GLFW_LIBRARY_DIRS ${GLFW_LIBRARY_DIRS}" )
  INCLUDE_DIRECTORIES(${GLFW_INCLUDE_DIRS})
  LINK_DIRECTORIES(${GLFW_LIBRARY_DIRS})
else()
  message (STATUS "glfw3 is not found." )
endif()

# Configure C++ compiler's includes dir
include_directories ( SYSTEM ${OpenCASCADE_INCLUDE_DIR} )

# Add executable
add_executable (Lesson_12_XDE
  DisplayScene.h
  DisplayScene.cpp
  main.cpp
  GlfwOcctViewer.cpp
  GlfwOcctViewer.h
  ViewerInteractor.cpp
  ViewerInteractor.h
  GlfwOcctWindow.h
  GlfwOcctWindow.cpp
)

# Add linker options
foreach (LIB ${OpenCASCADE_LIBRARIES})
  target_link_libraries(Lesson_12_XDE debug ${OpenCASCADE_LIBRARY_DIR}d/${LIB}.lib)
  target_link_libraries(Lesson_12_XDE optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
endforeach()

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
  TKLCAF
  TKBin
  TKBinL
  TKXCAF
  TKBinXCAF
  TKDESTEP
)

target_link_libraries (
  Lesson_12_XDE
  ${OpenCASCADE_LIBS}
  glfw
)
# Adjust runtime environment
set_property(TARGET Lesson_12_XDE PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%")
