cmake_minimum_required (VERSION 3.6 FATAL_ERROR)

# Project name
project(extras_MemLeaks CXX)

# OpenCascade
find_package(OpenCASCADE)

# Configure C++ compiler's includes dir
include_directories( SYSTEM ${OpenCASCADE_INCLUDE_DIR} )

# Add executable
add_executable(extras_MemLeaks
  main.cpp
  MemTracker.h
  MemTracker.cpp
)

# Add linker options
foreach (LIB ${OpenCASCADE_LIBRARIES})
  target_link_libraries(extras_MemLeaks debug ${OpenCASCADE_LIBRARY_DIR}d/${LIB}.lib)
  target_link_libraries(extras_MemLeaks optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
endforeach()

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

# Adjust runtime environment
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT extras_MemLeaks)
set_property(TARGET extras_MemLeaks PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%")
