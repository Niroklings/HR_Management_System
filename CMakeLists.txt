cmake_minimum_required(VERSION 3.10)
project(KursovayaProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(MYSQL_INCLUDE_DIRS
    "/usr/include/mysql-cppconn/jdbc" 
)
find_path(MYSQLCPPCONN_INCLUDE_DIR
    NAMES mysql_driver.h
    PATHS /usr/local/include/mysql /usr/include/mysql-cppconn-8/jdbc
    REQUIRED
)

if(NOT MYSQLCPPCONN_INCLUDE_DIR)
    message(FATAL_ERROR "MySQL Connector/C++ headers not found! Tried paths: ${MYSQL_INCLUDE_DIRS}")
endif()

include_directories(${MYSQLCPPCONN_INCLUDE_DIR})

set(SOURCES
    src/Accountant.cpp
    src/DatabaseManager.cpp
    src/Director.cpp
    src/Driver.cpp
    src/HRAnalyzer.cpp
    src/IdentifierGenerator.cpp
    src/main.cpp
    src/Programmer.cpp
    src/Secretary.cpp
)

add_executable(program ${SOURCES})

find_library(MYSQLCPPCONN_LIB
    NAMES mysqlcppconn mysqlcppconn8
    PATHS /usr/local/lib /usr/lib/x86_64-linux-gnu
    REQUIRED
)


if(NOT MYSQLCPPCONN_LIB)
    message(FATAL_ERROR "MySQL Connector/C++ library not found!")
endif()

target_link_libraries(program PRIVATE ${MYSQLCPPCONN_LIB})