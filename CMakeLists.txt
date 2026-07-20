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

add_executable(foosball_tracker
    src/main.cpp)
target_include_directories(foosball_tracker
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/players-detector
)
target_link_libraries(foosball_tracker
    PUBLIC
    ${OpenCV_LIBS}
    players_detector
)