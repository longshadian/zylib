cmake_minimum_required(VERSION 3.0)
project(zylib_exerimental)
set(OUTPUT zylib_exerimental)

message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
if(FALSE)
    set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/output)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/debug/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/debug/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/debug/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/release/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/release/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/release/lib)
endif()

file(GLOB SRC	./*.cpp ./*.h)

source_group("" FILES ${SRC})

SET(SRC_LIST 
    ${SRC} 
)

#find_package(Boost COMPONENTS filesystem system REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})

add_definitions("/std:c++17")
add_definitions("/D _WIN32_WINNT=0x0601")
add_definitions("/D _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
add_definitions("/D _CRT_SECURE_NO_WARNINGS")
add_definitions("/utf-8")     ### utf-8 no-bom warning C4819: 
#add_definitions("/D KNET_EXPORT=__declspec(dllexport)")
include_directories(${CMAKE_HOME_DIRECTORY})

add_executable(${OUTPUT} ${SRC_LIST})
#target_link_libraries(${OUTPUT})
