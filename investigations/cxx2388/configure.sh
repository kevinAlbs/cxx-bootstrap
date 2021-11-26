cmake \
    -DCMAKE_PREFIX_PATH=../../install/mongo-cxx-driver-master \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_C_COMPILER_LAUNCHER="ccache" \
    -DCMAKE_C_FLAGS="-fsanitize=address" \
    -DCMAKE_CXX_STANDARD=17 \
    -S./ \
    -B./cmake-build
