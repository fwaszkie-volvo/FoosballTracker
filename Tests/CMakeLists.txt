cmake_minimum_required(VERSION 3.14)
project(FoosballTrackerTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

enable_testing()

include(GoogleTest)

find_package(GTest REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenCV REQUIRED)

set(FOOSBALL_APP_MODEL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../App/Model")

if(EXISTS "${FOOSBALL_APP_MODEL_SOURCE_DIR}/CMakeLists.txt")
    add_subdirectory("${FOOSBALL_APP_MODEL_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/App/Model")
else()
    message(FATAL_ERROR "Nie znaleziono App/Model/CMakeLists.txt pod sciezka: ${FOOSBALL_APP_MODEL_SOURCE_DIR}")
endif()

add_executable(unit_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_main.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/readers_test.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/frame_processor_test.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/ball_detector_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/players_detector_test.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/kick_detector_test.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/generator_test.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/match_storage_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../App/log_level_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/utils.cpp
)
target_include_directories(unit_tests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../App
    ${FOOSBALL_APP_MODEL_SOURCE_DIR}/include/public
    ${FOOSBALL_APP_MODEL_SOURCE_DIR}/include/private
    ${FOOSBALL_APP_MODEL_SOURCE_DIR}/include/private/detectors
    ${FOOSBALL_APP_MODEL_SOURCE_DIR}/include/private/utils
    ${FOOSBALL_APP_MODEL_SOURCE_DIR}/include/private/readers
)

target_compile_options(unit_tests PRIVATE -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

target_compile_definitions(unit_tests
    PRIVATE
    TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
)

target_link_libraries(unit_tests
    PRIVATE
      foosball_model
      foosball_input_readers
      GTest::gtest
      spdlog::spdlog
      fmt::fmt
      ${OpenCV_LIBS}
)
gtest_discover_tests(unit_tests)
