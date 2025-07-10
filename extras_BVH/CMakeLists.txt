cmake_minimum_required (VERSION 3.6 FATAL_ERROR)

# Project name
project(extras_BVH CXX)

# OpenCascade
find_package(OpenCASCADE)

# Configure C++ compiler's includes dir
include_directories( SYSTEM ${OpenCASCADE_INCLUDE_DIR} )

# Add executable
add_executable(extras_BVH
  BVHFacets.cpp
  BVHFacets.h
  BVHIterator.h
  BVHIterator.cpp
  main.cpp
)

# Add linker options
foreach (LIB ${OpenCASCADE_LIBRARIES})
  target_link_libraries(extras_BVH debug ${OpenCASCADE_LIBRARY_DIR}d/${LIB}.lib)
  target_link_libraries(extras_BVH optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
endforeach()

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

# Adjust runtime environment
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT extras_BVH)
set_property(TARGET extras_BVH PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%")
