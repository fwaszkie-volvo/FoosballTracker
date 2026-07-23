cmake_minimum_required(VERSION 3.14)
project(FoosballTrackerTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include(GoogleTest)

find_package(GTest REQUIRED)

add_executable(unit_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/readers_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frame_processor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ball_detector_test.cpp
)
target_include_directories(unit_tests
    PRIVATE
    ${FOOSBALL_APP_MODEL_SOURCE_DIR}/Model/include/private
    ${FOOSBALL_APP_MODEL_SOURCE_DIR}/Model/include/private/detectors
    ${FOOSBALL_APP_MODEL_SOURCE_DIR}/Model/include/private/utils
    ${FOOSBALL_APP_MODEL_SOURCE_DIR}/Model/include/private/readers
)
target_compile_definitions(unit_tests
    PRIVATE
    TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

set(FOOSBALL_APP_MODEL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../App/Model")
if(EXISTS "${FOOSBALL_APP_MODEL_SOURCE_DIR}/CMakeLists.txt")
    add_subdirectory("${FOOSBALL_APP_MODEL_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/App/Model")
else()
    message(FATAL_ERROR "Nie znaleziono App/Model/CMakeLists.txt pod sciezka: ${FOOSBALL_APP_MODEL_SOURCE_DIR}")
endif()

target_link_libraries(unit_tests
    PRIVATE
      foosball_model
      foosball_input_readers
      GTest::gtest_main
)
gtest_discover_tests(unit_tests)
