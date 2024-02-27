cmake_minimum_required (VERSION 3.6 FATAL_ERROR)

# Project name
project(Lesson23_HLR CXX)

# OpenCascade
find_package(OpenCASCADE)

# Configure C++ compiler's includes dir
include_directories( SYSTEM ${OpenCASCADE_INCLUDE_DIR} )

# Add executable
add_executable(Lesson23_HLR
  main.cpp
  Timer.h
  Viewer.cpp
  Viewer.h
  ViewerInteractor.cpp
  ViewerInteractor.h
)

# Add linker options
foreach (LIB ${OpenCASCADE_LIBRARIES})
  target_link_libraries(Lesson23_HLR debug ${OpenCASCADE_LIBRARY_DIR}d/${LIB}.lib)
  target_link_libraries(Lesson23_HLR optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
endforeach()

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

# Adjust runtime environment
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Lesson23_HLR)
set_property(TARGET Lesson23_HLR PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%")
