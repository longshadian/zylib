cmake_minimum_required(VERSION 3.0)
project(zylibsoft)
set(OUTPUT zylibsoft)

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

file(GLOB SRC	./*.cpp ./*.h)
file(GLOB SRC_ZYLIBSOFT zylibsoft/*.cpp zylibsoft/*.h)
file(GLOB SRC_ZYLIBSOFT_STRING zylibsoft/string/*.cpp zylibsoft/string/*.h)

## test code
file(GLOB SRC_TEST test/*.cpp test/*.h)


source_group("" FILES ${SRC})
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SRC_ZYLIBSOFT})
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SRC_ZYLIBSOFT_STRING})
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SRC_TEST})

set(SRC_LIST 
    ${SRC_ZYLIBSOFT} 
    ${SRC_ZYLIBSOFT_STRING}

    ${SRC_TEST}
)

#find_package(Boost COMPONENTS filesystem system REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})

add_definitions("/std:c++17")
add_definitions("/D _WIN32_WINNT=0x0601")
add_definitions("/D _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
#add_definitions("/D _CRT_SECURE_NO_WARNINGS")
add_definitions("/utf-8")     ### utf-8 no-bom warning C4819: 
#add_definitions("/D KNET_EXPORT=__declspec(dllexport)")
include_directories(${CMAKE_HOME_DIRECTORY})

add_library(${OUTPUT} ${SRC_LIST})
#target_link_libraries(${OUTPUT})
