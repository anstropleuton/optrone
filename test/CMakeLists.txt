include("${OPTRONE_SOURCE_DIR}/cmake/subdep.cmake")

subdep_add(doctest)

set(OPTRONE_TESTS
    basic
)

foreach(TEST ${OPTRONE_TESTS})
    set(TEST_TARGET optrone_${TEST}_test)
    add_executable(${TEST_TARGET} ${TEST}.cpp)
    target_link_libraries(${TEST_TARGET} PRIVATE optrone doctest::doctest)
    target_include_directories(${TEST_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    set_target_properties(${TEST_TARGET} PROPERTIES OUTPUT_NAME ${TEST})
    add_test(NAME ${TEST} COMMAND ${TEST_TARGET})
endforeach()
