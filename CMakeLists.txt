cmake_minimum_required(VERSION 3.30)

project(optrone
    VERSION 1.0.0.1
    DESCRIPTION "Optrone is a simple argument parser for Anstro Pleuton's programs."
    LANGUAGES CXX
)

option(OPTRONE_BUILD_TESTS "Build Optrone tests" OFF)
option(OPTRONE_BUILD_EXAMPLES "Build Optrone examples" OFF)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/depman.cmake")

depman_declare(confer "https://github.com/anstropleuton/confer.git" origin/main)
depman_declare(alcelin "https://github.com/anstropleuton/alcelin.git" origin/main)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/optrone_config.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/optrone_config.hpp"
    @ONLY
)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pkgconfig.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/optrone.pc"
    @ONLY
)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in"
    "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    @ONLY
)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/header.html.in"
    "${CMAKE_CURRENT_BINARY_DIR}/header.html"
    @ONLY
)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/footer.html.in"
    "${CMAKE_CURRENT_BINARY_DIR}/footer.html"
    @ONLY
)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/optrone_config.cmake"
    @ONLY
)

# Taken from GLFW
# Only generate this target if no higher-level project already has
if (NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )

    add_custom_target(uninstall
        "${CMAKE_COMMAND}" -P
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    )
endif()

set(OPTRONE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/optrone.cpp"
)
set(OPTRONE_HEADERS
    "${CMAKE_CURRENT_BINARY_DIR}/optrone_config.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/optrone.hpp"
)
set(OPTRONE_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

add_library(optrone)
target_compile_features(optrone PUBLIC cxx_std_23)
target_sources(optrone PRIVATE ${OPTRONE_SOURCES})
target_sources(optrone PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${OPTRONE_INCLUDE_DIRS}
    FILES ${OPTRONE_HEADERS}
)

depman_make_available(alcelin)
target_link_libraries(optrone PUBLIC alcelin)

if(OPTRONE_BUILD_TESTS)
    depman_make_available(confer)
    add_subdirectory(tests)
endif()

if(OPTRONE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/optrone_config_version.cmake"
    COMPATIBILITY AnyNewerVersion
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/optrone_config_version.cmake"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/optrone"
)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/optrone.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
)
install(
    TARGETS optrone
    EXPORT optrone_targets
    FILE_SET HEADERS
)
install(
    EXPORT optrone_targets
    NAMESPACE optrone::
    DESTINATION "${CMAKE_INSTALL_DATADIR}/optrone"
)
