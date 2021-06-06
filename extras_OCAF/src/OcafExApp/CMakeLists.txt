project(OcafExApp)

#-------------------------------------------------------------------------------
# Name:    COPY_RESOURCES_TO_BINARY_DIRS
# Purpose: Copies resource files to all binary directories - bin, bind, bini
#-------------------------------------------------------------------------------
macro (COPY_RESOURCES_TO_BINARY_DIRS RESOURCE_FILEPATH)
  message (STATUS "... Copying resource file ${PROJECT_SOURCE_DIR}/${RESOURCE_FILEPATH}")

  if ( TARGET ${PROJECT_NAME} )
    add_custom_command( TARGET ${PROJECT_NAME}
                        POST_BUILD
                        COMMENT "Updating resource-file '${RESOURCE_FILEPATH}'"
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/${RESOURCE_FILEPATH}" "\$(OutDir)/${RESOURCE_FILEPATH}" )
  endif()

  foreach(CONF "RELEASE" "RELWITHDEBINFO" "DEBUG")
    file (COPY "${RESOURCE_FILEPATH}" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF}}/resources")
  endforeach()
endmacro()
#-------------------------------------------------------------------------------

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

# Group resource files in a sub-directory of IDE
source_group ("Resources" FILES resources/Plugin resources/Resources)

# Add executable
add_executable (OcafExApp ${SOURCES} ${HEADERS} resources/Plugin resources/Resources)

# Set executable dependent on OcafExLib
target_link_libraries(OcafExApp OcafExLib)

# Make resources available at binary directories
COPY_RESOURCES_TO_BINARY_DIRS(resources/Plugin)
COPY_RESOURCES_TO_BINARY_DIRS(resources/Resources)
