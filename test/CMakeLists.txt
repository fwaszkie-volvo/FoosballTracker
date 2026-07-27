include(GoogleTest)

find_package(GTest)

add_executable(readers_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/readers-test.cpp
)
target_include_directories(readers_tests
    PRIVATE
      ${CMAKE_SOURCE_DIR}/include
      ${CMAKE_SOURCE_DIR}/include/input
)
target_compile_definitions(readers_tests
    PRIVATE
    TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(readers_tests
    PRIVATE
      input_readers
      GTest::gtest_main
)
gtest_discover_tests(readers_tests)
