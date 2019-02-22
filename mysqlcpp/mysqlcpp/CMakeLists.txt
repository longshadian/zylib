CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS On)


###########################################################
## project output 
###########################################################
PROJECT(mysqlcpp)
SET(OUTPUT mysqlcpp)
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


###########################################################
## include 
###########################################################
INCLUDE_DIRECTORIES(${CMAKE_HOME_DIRECTORY})

INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/FindBoost_vcpkg.cmake)
INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIR})

INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/FindMysql_vcpkg.cmake)
INCLUDE_DIRECTORIES(${MYSQL_INCLUDE_DIR})


###########################################################
## cpp 
###########################################################
FILE(GLOB MYSQLCPP	./*.cpp ./*.h)
FILE(GLOB MYSQLCPP_DETAIL detail/*.cpp detail/*.h)

SOURCE_GROUP("" FILES ${KNET})
SOURCE_GROUP(TREE ${PROJECT_SOURCE_DIR} FILES ${MYSQLCPP})
SOURCE_GROUP(TREE ${PROJECT_SOURCE_DIR} FILES ${MYSQLCPP_DETAIL})

SET(SRC_LIST ${KNET} 
        ${MYSQLCPP} 
        ${MYSQLCPP_DETAIL} 
        )


###########################################################
## statis or dynamic
###########################################################
IF(KNET_SHARD)
    IF(WIN32)
        ADD_DEFINITIONS(/std:c++17)
        ADD_DEFINITIONS("/D _WIN32_WINNT=0x0601")
        ADD_DEFINITIONS("/D _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
        ADD_DEFINITIONS("/utf-8")     ### utf-8 no-bom warning C4819: 
        #ADD_DEFINITIONS("/D KNET_EXPORT=__declspec(dllexport)")
    ELSEIF(UNIX)
        #ADD_DEFINITIONS(-g -Wall -Wextra -Werror -std=c++17)
        #ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
        #SET(THIRD_LIBRARIES ${THIRD_LIBRARIES} pthread)
        ADD_DEFINITIONS(-g -Wall -Wextra  -std=c++17)
    ENDIF()

    ## third libraries
    SET(THIRD_LIBRARIES 
        ${BOOST_LIBRARIES}
        ${MYSQL_LIBRARIES}
        )
    ADD_LIBRARY(${OUTPUT} SHARED ${SRC_LIST})
    TARGET_LINK_LIBRARIES(${OUTPUT} ${THIRD_LIBRARIES})
ELSE()
        
ENDIF()

