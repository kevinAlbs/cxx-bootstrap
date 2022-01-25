cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(help30888 LANGUAGES C CXX)
find_package(mongocxx REQUIRED)
add_executable (main.out main.cpp)
target_link_libraries(main.out PRIVATE mongo::mongocxx_shared)

find_package(mongoc-1.0 REQUIRED)
add_executable(crepro.out crepro.c)
target_link_libraries(crepro.out PRIVATE mongo::mongoc_shared)