cmake_minimum_required(VERSION 3.14)
project(FoosballTracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV REQUIRED)

add_executable(foosball_tracker
    src/main.cpp
    src/detectors/ball-detector.cpp
    src/detectors/playfield-detector.cpp
    src/utils/mask-utils.cpp)
target_include_directories(foosball_tracker
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/detectors
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/detectors
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(foosball_tracker
    PUBLIC
    ${OpenCV_LIBS}
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Controller/CMakeLists.txt")
	add_subdirectory(Controller)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Model/CMakeLists.txt")
	add_subdirectory(Model)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/View/CMakeLists.txt")
	add_subdirectory(View)
endif()

add_executable(foosball_tracker main.cpp)
target_link_libraries(foosball_tracker PRIVATE ${OpenCV_LIBS})

if(TARGET foosball_controller)
	target_link_libraries(foosball_tracker PRIVATE foosball_controller)
endif()
