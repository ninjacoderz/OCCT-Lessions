cmake_minimum_required (VERSION 3.6 FATAL_ERROR)

# Project name
project(marathon CXX)

# Set a directory containing all 3-rd parties
set (3RDPARTY_DIR "" CACHE PATH "The root directory for all 3-rd parties.")

#------------------------------------------------------------------------------
# OpenCascade
#------------------------------------------------------------------------------

find_package(OpenCASCADE)

# CASROOT=D:/Work/occt/install-occt781
set (CASROOT ${OpenCASCADE_DIR}/..)
message (STATUS "... CASROOT: ${CASROOT}")

#------------------------------------------------------------------------------
# Tcl/tk
#------------------------------------------------------------------------------

set (CMAKE_FIND_LIBRARY_SUFFIXES ".dll")

find_library(TCL_LIBRARY "tcl86" REQUIRED PATHS "${3RDPARTY_DIR}/tcltk-86-64" PATH_SUFFIXES "bin")

list (APPEND 3RDPARTY_DLL_PATH       "${TCL_LIBRARY}/..")
list (APPEND 3RDPARTY_DLL_PATH_DEBUG "${TCL_LIBRARY}/..")

#------------------------------------------------------------------------------
# freetype
#------------------------------------------------------------------------------

find_library(FREETYPE_LIBRARY "freetype" REQUIRED PATHS "${3RDPARTY_DIR}/freetype-2.5.5-vc14-64" PATH_SUFFIXES "bin")

list (APPEND 3RDPARTY_DLL_PATH       "${FREETYPE_LIBRARY}/..")
list (APPEND 3RDPARTY_DLL_PATH_DEBUG "${FREETYPE_LIBRARY}/..")

#------------------------------------------------------------------------------
# freeimage
#------------------------------------------------------------------------------

find_library(FREEIMAGE_LIBRARY "freeimage" REQUIRED PATHS "${3RDPARTY_DIR}/freeimage-3.17.0-vc14-64" PATH_SUFFIXES "bin")

list (APPEND 3RDPARTY_DLL_PATH       "${FREEIMAGE_LIBRARY}/..")
list (APPEND 3RDPARTY_DLL_PATH_DEBUG "${FREEIMAGE_LIBRARY}/..")

# Configure C++ compiler's includes dir
include_directories( SYSTEM ${OpenCASCADE_INCLUDE_DIR} )

# Add executable
add_executable(marathon
  main.cpp
)

# Add linker options
foreach (LIB ${OpenCASCADE_LIBRARIES})
  target_link_libraries(marathon debug ${OpenCASCADE_LIBRARY_DIR}d/${LIB}.lib)
  target_link_libraries(marathon optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
endforeach()

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

set (3RDPARTY_DLL_PATH "${3RDPARTY_DLL_PATH};${OpenCASCADE_BINARY_DIR}")
set (3RDPARTY_DLL_PATH_DEBUG "${3RDPARTY_DLL_PATH_DEBUG};${OpenCASCADE_BINARY_DIR}d")

message(STATUS "3RDPARTY_DLL_PATH: ${3RDPARTY_DLL_PATH}")
message(STATUS "3RDPARTY_DLL_PATH_DEBUG: ${3RDPARTY_DLL_PATH_DEBUG}")

# Adjust runtime environment
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT marathon)
set_property(TARGET marathon PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=$<$<CONFIG:DEBUG>:${3RDPARTY_DLL_PATH_DEBUG}>$<$<NOT:$<CONFIG:DEBUG>>:${3RDPARTY_DLL_PATH}>;%PATH%
CASROOT=${OpenCASCADE_DIR}/..
")
