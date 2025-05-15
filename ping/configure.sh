cmake \
    -DCMAKE_PREFIX_PATH=../install/mongo-cxx-driver-r4.0.0 \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_CXX_STANDARD=17 \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -S./ \
    -B./cmake-build
