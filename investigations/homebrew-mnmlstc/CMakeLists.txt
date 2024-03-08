cmake_minimum_required(VERSION 3.15)

project(test_mongocxx LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)

# NOTE: For this to work, the CMAKE_PREFIX_PATH variable must be set to point to the directory that
# was used as the argument to CMAKE_INSTALL_PREFIX when building libmongocxx.
find_package(mongocxx REQUIRED)
add_executable(test_mongocxx test.cpp)
target_link_libraries(test_mongocxx PRIVATE mongo::mongocxx_shared) # Includes paths for MNMMLSTC headers.
