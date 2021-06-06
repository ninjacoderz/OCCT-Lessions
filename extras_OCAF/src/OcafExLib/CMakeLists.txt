project(OcafExLib)

# Add sources
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

# OpenCascade
find_package(OpenCASCADE)

# Configure C++ compiler's includes dir
include_directories ( SYSTEM ${CMAKE_CURRENT_SOURCE_DIR} ${OpenCASCADE_INCLUDE_DIR} )

# Add library
add_library (OcafExLib SHARED ${SOURCES} ${HEADERS})

# Add linker options
foreach (LIB ${OpenCASCADE_LIBRARIES})
  target_link_libraries(OcafExLib debug ${OpenCASCADE_LIBRARY_DIR}d/${LIB}.lib)
  target_link_libraries(OcafExLib optimized ${OpenCASCADE_LIBRARY_DIR}/${LIB}.lib)
endforeach()
