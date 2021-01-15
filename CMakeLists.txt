cmake_minimum_required (VERSION 3.0)

project(bootstrap LANGUAGES C CXX)

find_package(mongocxx REQUIRED)
add_executable(bootstrap bootstrap.cpp util.cpp)
add_executable(help20788 ./investigations/help20788.cpp util.cpp)
add_executable(initializer_list_constructor ./investigations/initializer_list_constructor.cpp)
target_link_libraries(bootstrap PRIVATE mongo::mongocxx_shared)
target_link_libraries(help20788 PRIVATE mongo::mongocxx_shared)
target_link_libraries(initializer_list_constructor PRIVATE mongo::mongocxx_shared)
