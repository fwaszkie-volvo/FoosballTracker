cmake_minimum_required(VERSION 3.14)
project(FoosballTracker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV REQUIRED)

add_executable(foosball_tracker main.cpp)
target_link_libraries(foosball_tracker PRIVATE ${OpenCV_LIBS})