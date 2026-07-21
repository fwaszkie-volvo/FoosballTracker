cmake_minimum_required(VERSION 3.14)

project(FoosballTrackerModel LANGUAGES CXX)

add_library(foosball_view STATIC
	src/view_main.cpp
)

target_include_directories(foosball_view
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include/public
		${CMAKE_CURRENT_SOURCE_DIR}/include/private
)