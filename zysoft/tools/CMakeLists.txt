cmake_minimum_required(VERSION 3.0)
set_property(GLOBAL PROPERTY USE_FOLDERS On)

project(STLSoft)
set(OUTPUT STLSoft)

set(CMAKE_MODULE_PATH
    "${CMAKE_HOME_DIRECTORY}/cmake"
    ${CMAKE_MODULE_PATH}
    )

#### cmake模块
include(Functions)

#### 工程文件目录结构
set(SRC_DIR "${CMAKE_HOME_DIRECTORY}/STLSoft/include")
auto_sources(SRC_FILES "*.cpp" "RECURSE" "${SRC_DIR}")
auto_sources(SRC_HFILES "*.h" "RECURSE" "${SRC_DIR}")
auto_source_group(STLSoft ${SRC_DIR} ${SRC_FILES} ${SRC_HFILES})

#### 编译选项
if(WIN32)
    add_definitions(/std:c++17)
    add_definitions("/D _WIN32_WINNT=0x0601")
    add_definitions("/D _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
    add_definitions("/D _CRT_SECURE_NO_WARNINGS")
    add_definitions("/utf-8")     ### utf-8 no-bom warning C4819: 
    #set（CMAKE_MSVCIDE_RUN_PATH $ {LibFoo_RUNTIME_LIBRARY_DIRS}）
    
elseif(UNIX)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Wconversion -Wshadow -Werror -std=c++17 -DNDEBUG")	
endif()

include_directories("${CMAKE_HOME_DIRECTORY}/STLSoft/include")
add_executable(${OUTPUT} ${SRC_FILES} ${SRC_HFILES})
