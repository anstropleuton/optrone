set(OPTRONE_EXAMPLES
    taskmgr
)

foreach(EXAMPLE ${OPTRONE_EXAMPLES})
    set(EXAMPLE_TARGET optrone_${EXAMPLE}_example)
    add_executable(${EXAMPLE_TARGET} ${EXAMPLE}.cpp)
    target_link_libraries(${EXAMPLE_TARGET} PRIVATE optrone)
    target_include_directories(${EXAMPLE_TARGET} PRIVATE ${OPTRONE_SOURCE_DIR}/example)
    set_target_properties(${EXAMPLE_TARGET} PROPERTIES OUTPUT_NAME ${EXAMPLE})
endforeach()
