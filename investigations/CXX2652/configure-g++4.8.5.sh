CMAKE=/root/cmake-3.25.2-linux-x86_64/bin/cmake
$CMAKE \
    -DCMAKE_PREFIX_PATH=/root/install/mongo-cxx-driver-3.7.0 \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_CXX_STANDARD=17 \
    -S./ \
    -B./cmake-build-g++4.8.5
