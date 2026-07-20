include(GoogleTest)

add_compile_definitions(TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(test
     ${CMAKE_CURRENT_SOURCE_DIR}/players-detector/players-detector-test.cpp
)
target_include_directories(test
    PRIVATE
     ${CMAKE_SOURCE_DIR}/include/players-detector
)
target_link_libraries(test
    players_detector
    gtest_main
)