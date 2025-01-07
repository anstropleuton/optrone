set(OPTRONE_EXAMPLES
    "example"
)

function(add_example source executable)
    add_executable(${executable} ${source})
    target_link_libraries(${executable} optrone)
endfunction()

foreach(example ${OPTRONE_EXAMPLES})
    set(example_source "${CMAKE_CURRENT_SOURCE_DIR}/${example}.cpp")
    string(REPLACE "/" "_" example_executable optrone_${example})
    add_example(${example_source} ${example_executable})
endforeach()
