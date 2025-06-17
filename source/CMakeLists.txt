add_library(optrone
    optrone.cpp
    parser.cpp
    help.cpp
)
target_include_directories(optrone PUBLIC
    $<BUILD_INTERFACE:${OPTRONE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(optrone PUBLIC cxx_std_23)

install(TARGETS optrone
    EXPORT optrone_EXPORT
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY "${OPTRONE_SOURCE_DIR}/include" DESTINATION "include")
install(EXPORT optrone_EXPORT
    FILE "optrone-config.cmake"
    NAMESPACE optrone::
    DESTINATION "lib/cmake/optrone"
)
export(TARGETS optrone
    FILE "${OPTRONE_BINARY_DIR}/optrone_export.cmake"
    NAMESPACE optrone::
)

configure_file(
    "${OPTRONE_SOURCE_DIR}/cmake/optrone.pc.in"
    "${OPTRONE_BINARY_DIR}/cmake/optrone.pc"
    @ONLY
)
install(FILES "${OPTRONE_BINARY_DIR}/cmake/optrone.pc" DESTINATION "lib/pkgconfig")

add_executable(optrone_executable
    main.cpp
)
target_link_libraries(optrone_executable PRIVATE optrone)

install(TARGETS optrone_executable
    RUNTIME DESTINATION bin
)
