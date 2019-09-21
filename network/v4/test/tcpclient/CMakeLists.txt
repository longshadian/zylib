CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS On)

project(TcpClient)
set(OUTPUT TcpClient)
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/output)
option(KNET_SHARD "output type is shared" ON)

if(WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/debug/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/debug/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/debug/bin)

    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/release/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/release/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/release/lib)
elseif(UNIX)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/bin)
endif(WIN32)

set(SRC_LIST 
    TestTcpClient.cpp
        )
		
include_directories(E:\\vcpkgVS2017\\vcpkg\\installed\\x64-windows\\include)		
include_directories(${CMAKE_HOME_DIRECTORY})
include_directories("../../")

    
if(WIN32)
    add_definitions(/std:c++17)
    add_definitions("/D _WIN32_WINNT=0x0601")
    add_definitions("/D _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
    add_definitions("/utf-8")     ### utf-8 no-bom warning C4819: 
    add_definitions("/W4")

elseif(UNIX)
    #ADD_DEFINITIONS(-g -Wall -Wextra -Werror -std=c++17)
    #ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
    #SET(THIRD_LIBRARIES ${THIRD_LIBRARIES} pthread)
    add_definitions(-g -Wall -Wextra  -std=c++17)
endif()

add_executable(${OUTPUT} ${SRC_LIST})
TARGET_LINK_LIBRARIES(${OUTPUT} Network4)
