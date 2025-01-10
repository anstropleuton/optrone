set(OPTRONE_TESTS
    "${CMAKE_CURRENT_SOURCE_DIR}/test_0.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_1.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_2.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_3.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_4.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_5.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_6.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_7.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_8.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_9.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_10.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_11.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_12.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/tester.cpp"
)

add_executable(optrone_tester ${OPTRONE_TESTS})
target_include_directories(optrone_tester PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(optrone_tester PRIVATE optrone)
target_link_libraries(optrone_tester PRIVATE confer)
