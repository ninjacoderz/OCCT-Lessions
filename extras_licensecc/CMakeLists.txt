cmake_minimum_required (VERSION 3.6 FATAL_ERROR)

# Project name
project(extras_licensecc CXX)

# OpenCascade
find_package(OpenCASCADE)

# Configure C++ compiler's includes dir
include_directories( SYSTEM ${OpenCASCADE_INCLUDE_DIR} )

add_definitions (-DUSE_LICENSECC)

if (NOT DEFINED 3RDPARTY_licensecc_DIR)
  set (3RDPARTY_licensecc_DIR ""
       CACHE PATH
       "The directory containing Licensecc")
endif()

set (3RDPARTY_licensecc_INCLUDE_DIR "${3RDPARTY_licensecc_DIR}/include/licensecc" CACHE PATH "The directory containing Licensecc." FORCE)
set (3RDPARTY_licensecc_LIBRARY_DIR "${3RDPARTY_licensecc_DIR}/licensecc" CACHE FILEPATH "" FORCE)

include_directories( SYSTEM ${3RDPARTY_licensecc_INCLUDE_DIR};${3RDPARTY_licensecc_INCLUDE_DIR}/DEMO_PROJECT )

# Add executable
add_executable(extras_licensecc
  main.cpp
)

# Add linker options
foreach (LIB ${OpenCASCADE_LIBRARIES})
  target_link_libraries(extras_licensecc debug ${OpenCASCADE_LIBRARY_DIR}d/${LIB}.lib)
  target_link_libraries(extras_licensecc optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
endforeach()

target_link_libraries (extras_licensecc debug ${3RDPARTY_licensecc_LIBRARY_DIR}/DEMO_PROJECT/debug/licensecc_static.lib)
target_link_libraries (extras_licensecc optimized ${3RDPARTY_licensecc_LIBRARY_DIR}/DEMO_PROJECT/release/licensecc_static.lib)
target_link_libraries (extras_licensecc bcrypt.lib)

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

# Adjust runtime environment
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT extras_licensecc)
set_property(TARGET extras_licensecc PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%")
