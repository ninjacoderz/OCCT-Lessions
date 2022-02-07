cmake_minimum_required (VERSION 3.0.0 FATAL_ERROR)

# Project name
project (Lesson_04 CXX)

# OpenCascade
find_package(OpenCASCADE)

# Qt
find_package(Qt)

# Configure C++ compiler's includes dir
include_directories ( SYSTEM ${OpenCASCADE_INCLUDE_DIR} )

# Add executable
add_executable (Lesson_04
  main.cpp
  Viewer.cpp
  Viewer.h
  ViewerInteractor.cpp
  ViewerInteractor.h
)

# Add linker options
foreach (LIB ${OpenCASCADE_LIBRARIES})
  target_link_libraries(Lesson_04 debug ${OpenCASCADE_LIBRARY_DIR}d/${LIB}.lib)
  target_link_libraries(Lesson_04 optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
endforeach()

# Adjust runtime environment
set_property(TARGET Lesson_04 PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%")
