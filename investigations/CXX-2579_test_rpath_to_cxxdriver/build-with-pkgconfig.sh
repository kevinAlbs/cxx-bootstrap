export PKG_CONFIG_PATH=~/code/cxx-bootstrap/install/mongo-cxx-driver-r3.7.0/lib/pkgconfig

clang++ -std=c++17 $(pkg-config --cflags --libs libmongocxx) -o ./app.out app.cpp