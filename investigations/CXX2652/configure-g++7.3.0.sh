CMAKE=/root/cmake-3.25.2-linux-x86_64/bin/cmake
# Configure with -D_GLIBCXX_USE_CXX11_ABI=0 to work around this error:
#   CMakeFiles/main.out.dir/main.cpp.o: In function `main':
#   /root/cxx-bootstrap/investigations/CXX2652/main.cpp:125: undefined reference to `mongocxx::v_noabi::uri::k_default_uri[abi:cxx11]'
#   collect2: error: ld returned 1 exit status
$CMAKE \
    -DCMAKE_PREFIX_PATH=/root/install/mongo-cxx-driver-3.7.0 \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_CXX_FLAGS="-D_GLIBCXX_USE_CXX11_ABI=0" \
    -DCMAKE_CXX_STANDARD=17 \
    -DCMAKE_CXX_COMPILER=/root/GCC-7.3.0/bin/g++ \
    -S./ \
    -B./cmake-build-g++7.3.0
