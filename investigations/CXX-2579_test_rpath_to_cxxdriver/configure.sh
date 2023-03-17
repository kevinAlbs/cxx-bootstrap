cmake \
    -DCMAKE_PREFIX_PATH=~/code/cxx-bootstrap/install/mongo-cxx-driver-r3.7.0 \
    -DCMAKE_INSTALL_PREFIX=$(pwd)/install \
    -DCMAKE_CXX_STANDARD=17 \
    -S./ \
    -B./cmake-build
