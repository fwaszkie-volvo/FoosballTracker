cmake_minimum_required(VERSION 3.14)
project(FoosballTrackerWorkspace LANGUAGES CXX)

include(CTest)

add_subdirectory(App)

if(BUILD_TESTING)
    add_subdirectory(Tests)
endif()
