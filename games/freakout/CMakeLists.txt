CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS On)


###########################################################
## project output 
###########################################################
PROJECT(freakout)
SET(OUTPUT freakout)
SET(OUTPUT_DIR ${CMAKE_BINARY_DIR}/output)
OPTION(MYSQLCPP_SHARD "output type is shared" ON)

IF(WIN32)
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/debug/lib)
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/debug/lib)
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/debug/bin)

    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/release/lib)
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/release/bin)
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/release/lib)
ELSEIF(UNIX)
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}/lib)
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/lib)
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/bin)
ENDIF(WIN32)


#SET(CMAKE_TOOLCHAIN_FILE C:/vcpkg/scripts/buildsystems/vcpkg.cmake)
###########################################################
## include 
###########################################################
INCLUDE_DIRECTORIES(${CMAKE_HOME_DIRECTORY})

#INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/FindVcpkg.cmake)

#INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/FindBoost_vcpkg.cmake)
#INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIR})

#INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/FindMysql_vcpkg.cmake)
#INCLUDE_DIRECTORIES(${MYSQL_INCLUDE_DIR})


###########################################################
## cpp 
###########################################################
FILE(GLOB SRC_FREAKOUT	./*.cpp ./*.h)


SOURCE_GROUP("" FILES ${SRC_FREAKOUT})

SET(SRC_LIST ${SRC_FREAKOUT} 
        )


###########################################################
## statis or dynamic
###########################################################

ADD_DEFINITIONS(/std:c++17)
#ADD_DEFINITIONS("/D _WIN32_WINNT=0x0601")
#ADD_DEFINITIONS("/D _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
ADD_DEFINITIONS("/utf-8")     ### utf-8 no-bom warning C4819: 
ADD_DEFINITIONS(/wd4251)    ### protocolbuf warning


#TARGET_LINK_LIBRARIES(${OUTPUT})
ADD_EXECUTABLE(${OUTPUT} ${SRC_LIST})



