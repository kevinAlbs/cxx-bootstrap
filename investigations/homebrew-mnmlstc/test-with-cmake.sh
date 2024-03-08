export CMAKE_PREFIX_PATH=$(pwd)/KEVINALBS/install-3.9.0
cmake -S. -Bcmake-build
cmake --build cmake-build --target test_mongocxx
