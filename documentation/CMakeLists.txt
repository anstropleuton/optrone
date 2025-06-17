find_package(Doxygen QUIET)
if(Doxygen_FOUND)
    configure_file(
        "${OPTRONE_SOURCE_DIR}/documentation/Doxyfile.in"
        "${OPTRONE_BINARY_DIR}/documentation/Doxyfile"
        @ONLY
    )
    add_custom_target(documentation
        COMMAND ${CMAKE_COMMAND} -E echo "Generating documentation..."
        COMMAND ${DOXYGEN_EXECUTABLE} "${OPTRONE_BINARY_DIR}/documentation/Doxyfile"
        WORKING_DIRECTORY "${OPTRONE_SOURCE_DIR}/documentation"
        COMMENT "Generate API documentation with Doxygen"
        VERBATIM
    )
endif()
