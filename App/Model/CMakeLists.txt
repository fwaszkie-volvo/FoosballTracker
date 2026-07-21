cmake_minimum_required(VERSION 3.14)

project(FoosballTrackerModel LANGUAGES CXX)

find_package(OpenCV REQUIRED)

add_library(foosball_model STATIC
	src/model_main.cpp
	src/detectors/ball_detector.cpp
	src/detectors/playfield_detector.cpp
	src/utils/mask_utils.cpp
)

target_link_libraries(foosball_model
	PUBLIC
		${OpenCV_LIBS}
)

target_include_directories(foosball_model
	PUBLIC
		${OpenCV_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}/include/public
		${CMAKE_CURRENT_SOURCE_DIR}/include/private/detectors
		${CMAKE_CURRENT_SOURCE_DIR}/include/private/utils
)