
IF(WIN32)
    FIND_PATH(MYSQL_INCLUDE_DIR mysql.h
        PATHS
        E:\\libs\\mysql-connector-c-6.1.11-src\\include
        
        NO_DEFAULT_PATH
    )
ELSEIF(UNIX)
    FIND_PATH(MYSQL_INCLUDE_DIR mysql.h
        PATHS
        /usr/lib
        /usr/local/lib
        /usr/local/mysql/include
        
        NO_DEFAULT_PATH
    )
ENDIF(WIN32)

IF(NOT MYSQL_INCLUDE_DIR)
    MESSAGE(FATAL_ERROR "Can't find MYSQL_INCLUDE_DIR: ${MYSQL_INCLUDE_DIR}")
  ELSE()
    MESSAGE("Set MYSQL_INCLUDE_DIR: ${MYSQL_INCLUDE_DIR}")
ENDIF()


IF(WIN32)
    FIND_PATH(MYSQLCLIENT_LIBRARY_DIR libmysql.lib
        PATHS
        E:\\libs\\libmysqlclient\\libmysql\\Debug
        
        NO_DEFAULT_PATH
    )
ELSEIF(UNIX)
    FIND_PATH(MYSQLCLIENT_LIBRARY_DIR libmysqlclient.so
        PATHS
        /usr/lib
        /usr/local/lib
        /usr/local/mysql/lib
        
        NO_DEFAULT_PATH
    )
ENDIF(WIN32)

IF(NOT MYSQLCLIENT_LIBRARY_DIR)
    MESSAGE(FATAL_ERROR "Can't find MYSQLCLIENT_LIBRARY_DIR: ${MYSQLCLIENT_LIBRARY_DIR}")
  ELSE()
    MESSAGE("Set MYSQLCLIENT_LIBRARY_DIR: ${MYSQLCLIENT_LIBRARY_DIR}")
ENDIF()

IF(WIN32)
    FIND_LIBRARY(LIB_MYSQLCLIENT libmysql.lib PATHS ${MYSQLCLIENT_LIBRARY_DIR})
ELSEIF(UNIX)
    FIND_LIBRARY(LIB_MYSQLCLIENT libmysqlclient.so PATHS ${MYSQLCLIENT_LIBRARY_DIR})
ENDIF(WIN32)

IF(NOT LIB_MYSQLCLIENT)
    MESSAGE(FATAL_ERROR "Can't find LIB_MYSQLCLIENT")
ENDIF()

SET(MYSQL_LIBRARIES ${LIB_MYSQLCLIENT} CACHE STRING "mysql libraries")
