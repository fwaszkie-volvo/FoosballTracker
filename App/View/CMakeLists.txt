cmake_minimum_required(VERSION 3.14)

project(FoosballTrackerModel LANGUAGES CXX)

find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs)
find_package(Threads REQUIRED)

include(FetchContent)
FetchContent_Declare(
	httplib
	GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
	GIT_TAG v0.15.3
)
FetchContent_MakeAvailable(httplib)

add_library(foosball_view STATIC
	src/view_main.cpp
)

target_link_libraries(foosball_view
	PUBLIC
		spdlog::spdlog
		fmt::fmt
	PRIVATE
		httplib::httplib
		Threads::Threads
)

target_include_directories(foosball_view
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include/public
		${OpenCV_INCLUDE_DIRS}
)

target_compile_definitions(foosball_view
	PRIVATE
		FOOSBALL_FRONTEND_BUILD_DIR="${CMAKE_CURRENT_SOURCE_DIR}/frontend/build"
)

target_link_libraries(foosball_view
	PRIVATE
		${OpenCV_LIBS}
)