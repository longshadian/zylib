cmake_minimum_required(VERSION 3.0)

set(PROJECT_NAME zysfot_net)
project(${PROJECT_NAME})

set(CMAKE_MODULE_PATH "${CMAKE_HOME_DIRECTORY}/cmake" ${CMAKE_MODULE_PATH})

#### cmake模块
include(Functions)
include_directories("C:/vcpkg/installed/x64-windows/include")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
auto_sources(SRC_FILES "*.cpp" "RECURSE" "${SRC_DIR}")
auto_sources(SRC_HFILES "*.h" "RECURSE" "${SRC_DIR}")
auto_sources(SRC_HPPFILES "*.hpp" "RECURSE" "${SRC_DIR}")
auto_source_group(src ${SRC_DIR} ${SRC_FILES} ${SRC_HFILES} ${SRC_HPPFILES})

#find_package(Boost COMPONENTS filesystem system REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})

add_definitions("/std:c++17")
add_definitions("/D _WIN32_WINNT=0x0601")
add_definitions("/D _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
#add_definitions("/D _CRT_SECURE_NO_WARNINGS")
add_definitions("/D NOMINMAX")
add_definitions("/utf-8")     ### utf-8 no-bom warning C4819: 

add_executable(${PROJECT_NAME} ${SRC_FILES} ${SRC_HFILES} ${SRC_HPPFILES})
#add_library(${OUTPUT} ${SRC_LIST})
