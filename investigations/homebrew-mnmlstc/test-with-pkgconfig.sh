export PKG_CONFIG_PATH=$(pwd)/KEVINALBS/install-3.9.0/lib/pkgconfig/
FLAGS=$(pkg-config --libs --cflags libmongocxx)
# FLAGS includes path to `/include/bsoncxx/v_noabi/bsoncxx/third_party/mnmlstc`
g++ -o KEVINALBS/test test.cpp -std=c++11 $FLAGS
