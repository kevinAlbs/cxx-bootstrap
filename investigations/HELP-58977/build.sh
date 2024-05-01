export PKG_CONFIG_PATH=/Users/kevin.albertson/code/cxx-bootstrap/install/mongo-cxx-driver-r3.9.0/lib/pkgconfig

clang++ -std=c++17 -o repro.out $(pkg-config --libs --cflags libmongocxx) repro.cpp

