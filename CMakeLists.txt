cmake_minimum_required(VERSION 3.14)
project(FoosballTracker)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(foosball_tracker main.cpp)
target_link_libraries(foosball_tracker PRIVATE ${OpenCV_LIBS})