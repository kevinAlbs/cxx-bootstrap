cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(batch_size_0 LANGUAGES C CXX)
find_package(mongocxx REQUIRED)
add_executable (main.out main.cpp)
target_link_libraries(main.out PRIVATE mongo::mongocxx_shared)

add_executable (capture_cursor_id.out capture_cursor_id.cpp)
target_link_libraries(capture_cursor_id.out PRIVATE mongo::mongocxx_shared)
