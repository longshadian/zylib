cmake_minimum_required(VERSION 3.0)
project(TestZYSoft)
set(OUTPUT TestZYSoft)

# message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
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
file(GLOB SRC	./*.cpp ./*.h)

source_group("" FILES ${SRC})

set(SRC_LIST 
    ${SRC} 
)

include_directories(${CMAKE_HOME_DIRECTORY}/src)

add_definitions("/std:c++17")
add_definitions("/D _WIN32_WINNT=0x0601")
add_definitions("/D _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
add_definitions("/utf-8")     ### utf-8 no-bom warning C4819: 

add_executable(${OUTPUT} ${SRC_LIST})
#ADD_LIBRARY(${OUTPUT} ${SRC_LIST})
