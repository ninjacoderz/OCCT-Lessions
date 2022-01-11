cmake_minimum_required (VERSION 3.0.0 FATAL_ERROR)

# Project name
project (viewerLines CXX)

# OpenCascade
if (NOT DEFINED OpenCASCADE_DIR)
  find_package(OpenCASCADE)
else()
  find_package(OpenCASCADE PATHS ${OpenCASCADE_DIR} NO_DEFAULT_PATH)
endif()

# Configure C++ compiler's includes dir
include_directories ( SYSTEM ${OpenCASCADE_INCLUDE_DIR} )

if (NOT DEFINED ${3RDPARTY_QT_DIR} AND ${3RDPARTY_QT_DIR} STREQUAL "")
  set (3RDPARTY_QT_DIR "" CACHE PATH "The directory containing qt")
  message (FATAL_ERROR "Define 3RDPARTY_QT_DIR dir")
endif()
find_package(Qt5 QUIET COMPONENTS Widgets Quick Xml PATHS ${3RDPARTY_QT_DIR} NO_DEFAULT_PATH)

# Configure C++ compiler's includes dir
#set(PROJECT_INCLUDES "${Qt5Widgets_INCLUDE_DIRS}" "${Qt5Quick_INCLUDE_DIRS}" "${Qt5Xml_INCLUDE_DIRS}")
include_directories ("${Qt5Widgets_INCLUDE_DIRS}" "${Qt5Quick_INCLUDE_DIRS}" "${Qt5Xml_INCLUDE_DIRS}")

set(PROJECT_LIBRARIES_DEBUG "${Qt5Widgets_LIBRARIES}" "${Qt5Quick_LIBRARIES}" "${Qt5Xml_LIBRARIES}")
set(PROJECT_LIBRARIES_RELEASE "${Qt5Widgets_LIBRARIES}" "${Qt5Quick_LIBRARIES}" "${Qt5Xml_LIBRARIES}")

set(MOC_FILE
  viewerLines_actions.h
  viewerLines_widget.h
)

set (GENERATED_MOC_FILES)
foreach (FILE ${MOC_FILE})
  unset (MOC_FILE)
  qt5_wrap_cpp(MOC_FILE ${FILE})
  list(APPEND GENERATED_MOC_FILES ${MOC_FILE})
endforeach()

# Add executable
add_executable (viewerLines
  main.cpp
  viewerLines_actions.cpp
  viewerLines_actions.h
  viewerLines_linePresentation.cpp
  viewerLines_linePresentation.h
  viewerLines_scene.cpp
  viewerLines_scene.h
  viewerLines_presentations.cpp
  viewerLines_presentations.h
  viewerLines_widget.cpp
  viewerLines_widget.h
  ${GENERATED_MOC_FILES}
)

# Add linker options
foreach (LIB ${OpenCASCADE_LIBRARIES})
  target_link_libraries(viewerLines debug ${OpenCASCADE_LIBRARY_DIR}d/${LIB}.lib)
  target_link_libraries(viewerLines optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
endforeach()

# Add qt libraries
foreach (LIB ${PROJECT_LIBRARIES_DEBUG})
  target_link_libraries (viewerLines debug ${LIB})
endforeach()
foreach (LIB ${PROJECT_LIBRARIES_RELEASE})
  target_link_libraries (viewerLines optimized ${LIB})
endforeach()

# Adjust runtime environment
set_property(TARGET viewerLines PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%")
