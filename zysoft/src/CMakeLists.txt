cmake_minimum_required(VERSION 3.0)
project(zysoft)
set(OUTPUT zysoft)

message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
if(TRUE)
    set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/output)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/debug/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/debug/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/debug/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/release/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/release/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/release/lib)
endif()

## zysoft
set(CMAKE_MODULE_PATH "${CMAKE_HOME_DIRECTORY}/cmake" ${CMAKE_MODULE_PATH})

#### cmake模块
include(Functions)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
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
add_definitions("/utf-8")     ### utf-8 no-bom warning C4819: 

add_executable(${OUTPUT} ${SRC_FILES} ${SRC_HFILES} ${SRC_HPPFILES})
#add_library(${OUTPUT} ${SRC_LIST})
