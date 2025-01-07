# optrone_config.cmake
include(${CMAKE_CURRENT_LIST_DIR}/optrone_targets.cmake)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/optrone_config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/optrone_config_version.cmake
    DESTINATION ${CMAKE_INSTALL_DATADIR}/optrone
)
