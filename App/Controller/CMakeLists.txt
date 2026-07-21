cmake_minimum_required(VERSION 3.14)

project(FoosballTrackerController)


add_library(foosball_controller STATIC
	src/controller.cpp
)

target_link_libraries(foosball_controller
	PUBLIC
		foosball_model
		foosball_view
)

target_include_directories(foosball_controller
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/../Model/include/public
		${CMAKE_CURRENT_SOURCE_DIR}/../View/include/public
)