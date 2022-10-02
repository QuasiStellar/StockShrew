cmake_minimum_required(VERSION 3.23)
project(StockShrew)

set(CMAKE_CXX_STANDARD 23)

set(SOURCE_FILES main.cpp)
add_executable(StockShrew ${SOURCE_FILES})

include_directories(src)
add_subdirectory(src)

target_link_libraries(StockShrew src)

add_subdirectory(test)
