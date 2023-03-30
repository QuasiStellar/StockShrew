cmake_minimum_required(VERSION 3.25.1)
project(StockShrew)

set(CMAKE_CXX_STANDARD 23)

set(SOURCE_FILES main.cpp)
add_executable(StockShrew ${SOURCE_FILES})

include_directories(src)
add_subdirectory(src)
add_subdirectory(test)

target_link_libraries(StockShrew src)

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

find_package(Boost 1.81.0 COMPONENTS program_options REQUIRED)
target_link_libraries(StockShrew Boost::program_options)
