cmake_minimum_required (VERSION 3.0.0 FATAL_ERROR)

# Project name
project (Lesson_18_Qt_Widgets CXX)

# OpenCascade
message (STATUS "Processing OpenCascade 3-rd party")
# Make sure that the "OpenCascade_DIR" variable will point to <OpenCascade directory>\install\cmake
find_package(OpenCASCADE)

# Configure C++ compiler's includes dir
include_directories ( SYSTEM ${OpenCASCADE_INCLUDE_DIR} )

# Qt
message (STATUS "Processing Qt 3-rd party")

# If Qt is not explicitly installed, but downloaded and stored somewhere in PC 
# Set a variable in CMake GUI so that be able to select the directory where Qt is.
if (NOT DEFINED ${QT_DIR} AND ${QT_DIR} STREQUAL "")
  set (QT_DIR "" CACHE PATH "The directory containing Qt" FORCE)
endif()

# Now set CMAKE_PREFIX_PATH to point to local Qt installation.
# Without this setting find_package() will not work
# Otherwise find_package should work well.
set(CMAKE_PREFIX_PATH ${QT_DIR})

# Now we can apply standard CMake finder for Qt.
# We do this mostly to have qt5_wrap_cpp() function, setting the rules of MOC files generation, available.
# If Qt version > 5, qt_wrap_cpp() should be used instead. 
# https://doc.qt.io/qt-5/qtcore-cmake-qt5-wrap-cpp.html
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# Gather the .h files including Qt for further MOC files generation
set (H_UI_FILES
  ViewerWidget.h
)

# Generation MOC files itself
foreach (FILE ${H_UI_FILES})
  unset (MOC_FILE)
  qt5_wrap_cpp(MOC_FILE ${FILE})
  set (UI_MOC_FILES ${UI_MOC_FILES} ${MOC_FILE})
  
  # Grouping source files in IDE project generation
  source_group ("MOC Files"     FILES   "${MOC_FILE}")
  source_group ("Header Files"  FILES   "${FILE}")
endforeach (FILE)

# Add executable
add_executable (Lesson_18_Qt_Widgets
  main.cpp
  ViewerWidget.cpp
  Viewer.cpp
  Viewer.h
  ${H_UI_FILES}
  ${UI_MOC_FILES}
)

# Indication of directories with headers and libs
qt5_use_modules(Lesson_18_Qt_Widgets Widgets)

# Add linker options
foreach (LIB ${OpenCASCADE_LIBRARIES})
  target_link_libraries(Lesson_18_Qt_Widgets debug ${OpenCASCADE_LIBRARY_DIR}d/${LIB}.lib)
  target_link_libraries(Lesson_18_Qt_Widgets optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
endforeach()

# Adjust runtime environment
set_property(TARGET Lesson_18_Qt_Widgets PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%")
