# Set CXX_INSTALL_PREFIX to the install path of the C++ driver.
CXX_INSTALL_PREFIX=${CXX_INSTALL_PREFIX:-../../install/mongo-cxx-driver-r3.7.0}
cmake \
    -DCMAKE_PREFIX_PATH=$CXX_INSTALL_PREFIX \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_CXX_STANDARD=17 \
    -S./ \
    -B./cmake-build
