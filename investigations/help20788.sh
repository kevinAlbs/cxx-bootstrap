clang++ --std=c++11 help20788-2.cpp $(PKG_CONFIG_PATH=/Users/kevin.albertson/install/mongo-cxx-driver-help20788/lib/pkgconfig pkg-config --cflags --libs libmongocxx) -o execute_test
export DYLD_LIBRARY_PATH=/Users/kevin.albertson/install/mongo-cxx-driver-help20788/lib
./execute_test