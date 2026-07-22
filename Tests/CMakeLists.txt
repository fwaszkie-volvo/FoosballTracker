include(GoogleTest)

find_package(GTest)

add_executable(unit_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/readers_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frame_processor_test.cpp
)
target_include_directories(unit_tests
    PRIVATE
      ${CMAKE_SOURCE_DIR}/Model/include/private
      ${CMAKE_SOURCE_DIR}/Model/include/private/readers
)
target_compile_definitions(unit_tests
    PRIVATE
    TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(unit_tests
    PRIVATE
      foosball_model
      foosball_input_readers
      GTest::gtest_main
)
gtest_discover_tests(unit_tests)
