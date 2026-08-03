cmake_minimum_required(VERSION 3.14)
project(FoosballTracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Controller/CMakeLists.txt")
	add_subdirectory(Controller)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Model/CMakeLists.txt")
	add_subdirectory(Model)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/View/CMakeLists.txt")
	add_subdirectory(View)
endif()

add_executable(foosball_tracker
	main.cpp
	log_level_manager.cpp
)
target_link_libraries(foosball_tracker PRIVATE ${OpenCV_LIBS} spdlog::spdlog fmt::fmt)

if(TARGET foosball_controller)
	target_link_libraries(foosball_tracker PRIVATE foosball_controller)
endif()
