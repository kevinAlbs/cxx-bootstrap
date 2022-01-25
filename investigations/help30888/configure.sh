if [ ! -d cmake-build ]; then
    mkdir cmake-build
fi

pushd cmake-build
cmake \
    -DCMAKE_PREFIX_PATH=../../install/mongo-cxx-driver-r3.6.6 \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_CXX_STANDARD=17 \
    ..
popd
