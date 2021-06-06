cmake_minimum_required (VERSION 3.0.0 FATAL_ERROR)

# Project name
project (OcafEx CXX)

# Build configurations
set (CMAKE_CONFIGURATION_TYPES Release Debug RelWithDebInfo CACHE INTERNAL "" FORCE)

# Sub-projects
add_subdirectory(${CMAKE_SOURCE_DIR}/src/OcafExLib)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/OcafExApp)
