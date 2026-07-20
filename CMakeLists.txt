cmake_minimum_required(VERSION 3.14)
project(FoosballTracker)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_library(players_detector src/players-detector/players-detector.cpp)
target_include_directories(players_detector
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/players-detector
)
target_link_libraries(players_detector
    PUBLIC
    ${OpenCV_LIBS}
)

add_executable(foosball_tracker
    src/main.cpp
    src/detectors/ball-detector.cpp
    src/detectors/playfield-detector.cpp
    src/utils/mask-utils.cpp)
target_include_directories(foosball_tracker
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/players-detector
    ${CMAKE_CURRENT_SOURCE_DIR}/include/detectors
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/detectors
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(foosball_tracker
    PUBLIC
    ${OpenCV_LIBS}
    players_detector
)

add_subdirectory(test)
