cmake_minimum_required(VERSION 3.14)

project(FoosballTrackerModel LANGUAGES CXX)

find_package(OpenCV REQUIRED COMPONENTS core imgproc)

# Find GTK 4.1
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk4>=4.1)

add_library(foosball_view STATIC
	src/view_main.cpp
	src/buttons/load_button.cpp
	src/buttons/live_button.cpp
	src/buttons/anal_button.cpp
	src/buttons/save_button.cpp
)

target_link_libraries(foosball_view
	PUBLIC
		spdlog::spdlog
		fmt::fmt
)

target_include_directories(foosball_view
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include/public
		${CMAKE_CURRENT_SOURCE_DIR}/include/private
		${OpenCV_INCLUDE_DIRS}
		${GTK_INCLUDE_DIRS}
)

target_compile_options(foosball_view PRIVATE ${GTK_CFLAGS_OTHER})
target_link_libraries(foosball_view
	PUBLIC
		${GTK_LIBRARIES}
	PRIVATE
		${OpenCV_LIBS}
)