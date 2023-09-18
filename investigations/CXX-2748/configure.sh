# With changes from: https://github.com/mongodb/mongo-cxx-driver/pull/1024/files
# cmake \
#     -DCMAKE_PREFIX_PATH=/Users/kevin.albertson/review/mongo-cxx-driver-1024/cmake-build/.install \
#     -DCMAKE_CXX_STANDARD=17 \
#     -S./ \
#     -B./cmake-build

# With C++ driver r3.8.0
# cmake \
#     -DCMAKE_PREFIX_PATH=/Users/kevin.albertson/code/cxx-bootstrap/install/mongo-cxx-driver-r3.8.0 \
#     -DCMAKE_CXX_STANDARD=17 \
#     -S./ \
#     -B./cmake-build

# With C++ driver 569fea2b72dd42da4dbfe3f69f43a08c7a1d9da1
cmake \
    -DCMAKE_PREFIX_PATH=/Users/kevin.albertson/code/mongo-cxx-driver/cmake-build/.install \
    -DCMAKE_CXX_STANDARD=17 \
    -S./ \
    -B./cmake-build
