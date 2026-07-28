cmake_minimum_required(VERSION 3.14)

project(FoosballTrackerModel LANGUAGES CXX)

add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs videoio)

add_library(foosball_model STATIC
	src/model_main.cpp
	src/frame_processor.cpp
	src/generator.cpp
	src/ratings/player_ratings.cpp
	src/ratings/team_delta_calculator.cpp
	src/detectors/ball_detector.cpp
	src/detectors/goal_detector.cpp
	src/detectors/playfield_detector.cpp
	src/detectors/players_detector.cpp
	src/utils/mask_utils.cpp
)

add_library(foosball_input_readers STATIC
	src/readers/reader_factory.cpp
	src/readers/recording_reader.cpp
	src/readers/online_reader.cpp
	src/readers/photo_reader.cpp
	src/readers/video_frame_reader.cpp
)

target_link_libraries(foosball_model
	PUBLIC
		${OpenCV_LIBS}
		spdlog::spdlog
		fmt::fmt
)

target_link_libraries(foosball_input_readers
	PUBLIC
		${OpenCV_LIBS}
)

target_include_directories(foosball_model
	PUBLIC
		${OpenCV_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}/include/public
		${CMAKE_CURRENT_SOURCE_DIR}/include/private
		${CMAKE_CURRENT_SOURCE_DIR}/include/private/ratings
		${CMAKE_CURRENT_SOURCE_DIR}/include/private/detectors
		${CMAKE_CURRENT_SOURCE_DIR}/include/private/readers
		${CMAKE_CURRENT_SOURCE_DIR}/include/private/utils
)

target_include_directories(foosball_input_readers
	PUBLIC
		${OpenCV_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}/include/private/readers
)

target_link_libraries(foosball_model
	PUBLIC
		foosball_input_readers
)
