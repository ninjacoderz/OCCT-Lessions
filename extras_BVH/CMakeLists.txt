cmake_minimum_required (VERSION 3.6 FATAL_ERROR)

# Project name.
project(extras_BVH CXX)

# Add OpenCascade.
find_package(OpenCASCADE)

# Configure C++ compiler's includes dir.
include_directories( SYSTEM ${OpenCASCADE_INCLUDE_DIR} )

# Add Freetype.
set(FREETYPE_DIR ${FREETYPE_DIR} CACHE PATH "The directory containing Freetype includes")
set(ENV{FREETYPE_DIR} ${FREETYPE_DIR})
find_package(Freetype)
#
if (FREETYPE_FOUND)
  set(FREETYPE_LIBRARY_BIN ${FREETYPE_DIR}/bin)
endif()

# Add FreeImage.
set(FREEIMAGE_DIR ${FREEIMAGE_DIR} CACHE PATH "The directory of FreeImage installation")
#
if (FREEIMAGE_DIR)
  set(FREEIMAGE_LIBRARY_BIN ${FREEIMAGE_DIR}/bin)
else()
  message(FATAL_ERROR "Cannot locate FreeImage")
endif()

# Add executable.
add_executable(extras_BVH
  BVHFacets.cpp
  BVHFacets.h
  BVHIterator.h
  BVHIterator.cpp
  main.cpp
  Viewer.cpp
  Viewer.h
  ViewerInteractor.cpp
  ViewerInteractor.h
)

# Link OpenCascade.
foreach (LIB ${OpenCASCADE_LIBRARIES})
  target_link_libraries(extras_BVH debug ${OpenCASCADE_LIBRARY_DIR}d/${LIB}.lib)
  target_link_libraries(extras_BVH optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
endforeach()

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

# Adjust runtime environment
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT extras_BVH)
set_property(TARGET extras_BVH PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=$<$<CONFIG:DEBUG>:${FREEIMAGE_LIBRARY_BIN};${FREETYPE_LIBRARY_BIN};${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${FREEIMAGE_LIBRARY_BIN};${FREETYPE_LIBRARY_BIN};${OpenCASCADE_BINARY_DIR}>;%PATH%")
