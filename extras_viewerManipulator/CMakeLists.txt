cmake_minimum_required (VERSION 3.0.0 FATAL_ERROR)

# Project name
project (viewerManipulator CXX)

# OpenCascade
if (NOT DEFINED OpenCASCADE_DIR)
  find_package(OpenCASCADE)
else()
  find_package(OpenCASCADE PATHS ${OpenCASCADE_DIR} NO_DEFAULT_PATH)
endif()

# FreeType
if (NOT DEFINED 3RDPARTY_Freetype_DLL_DIR)
  set (3RDPARTY_Freetype_DLL_DIR "" CACHE PATH "The directory containing binaries of freetype")
  message (FATAL_ERROR "Define 3RDPARTY_Freetype_DLL_DIR dir")
endif()

# TBB
if (NOT DEFINED 3RDPARTY_TBB_DLL_DIR)
  set (3RDPARTY_TBB_DLL_DIR "" CACHE PATH "The directory containing binaries of tbb")
  message (FATAL_ERROR "Define 3RDPARTY_TBB_DLL_DIR dir")
endif()

# Configure C++ compiler's includes dir
include_directories ( SYSTEM ${OpenCASCADE_INCLUDE_DIR} )

if (NOT DEFINED ${3RDPARTY_QT_DIR} AND ${3RDPARTY_QT_DIR} STREQUAL "")
  set (3RDPARTY_QT_DIR "" CACHE PATH "The directory containing qt")
  message (FATAL_ERROR "Define 3RDPARTY_QT_DIR dir")
endif()
find_package(Qt5 QUIET COMPONENTS Widgets Quick Xml PATHS ${3RDPARTY_QT_DIR} NO_DEFAULT_PATH)

# Configure C++ compiler's includes dir
include_directories ("${Qt5Widgets_INCLUDE_DIRS}" "${Qt5Quick_INCLUDE_DIRS}" "${Qt5Xml_INCLUDE_DIRS}")

set(PROJECT_LIBRARIES_DEBUG "${Qt5Widgets_LIBRARIES}" "${Qt5Quick_LIBRARIES}" "${Qt5Xml_LIBRARIES}")
set(PROJECT_LIBRARIES_RELEASE "${Qt5Widgets_LIBRARIES}" "${Qt5Quick_LIBRARIES}" "${Qt5Xml_LIBRARIES}")

set(MOC_FILE
  ViewerWidget.h
)

set (GENERATED_MOC_FILES)
foreach (FILE ${MOC_FILE})
  unset (MOC_FILE)
  qt5_wrap_cpp(MOC_FILE ${FILE})
  list(APPEND GENERATED_MOC_FILES ${MOC_FILE})
endforeach()

# Add executable
add_executable (${PROJECT_NAME}
  main.cpp
  Viewer.cpp
  Viewer.h
  ViewerInteractor.cpp
  ViewerInteractor.h
  ViewerWidget.cpp
  ViewerWidget.h
  ${GENERATED_MOC_FILES}
)

# Add linker options
foreach (LIB ${OpenCASCADE_LIBRARIES})
  target_link_libraries(${PROJECT_NAME} debug ${OpenCASCADE_LIBRARY_DIR}d/${LIB}.lib)
  target_link_libraries(${PROJECT_NAME} optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
endforeach()

# Add qt libraries
foreach (LIB ${PROJECT_LIBRARIES_DEBUG})
  target_link_libraries (${PROJECT_NAME} debug ${LIB})
endforeach()
foreach (LIB ${PROJECT_LIBRARIES_RELEASE})
  target_link_libraries (${PROJECT_NAME} optimized ${LIB})
endforeach()

# Adjust runtime environment
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;${3RDPARTY_QT_DIR}/bin;${3RDPARTY_Freetype_DLL_DIR};${3RDPARTY_TBB_DLL_DIR};%PATH%")
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY VS_DEBUGGER_ENVIRONMENT "\nQTDIR=${3RDPARTY_QT_DIR}")
