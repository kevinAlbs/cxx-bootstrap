Steps to install the C++ driver on Windows without internet access.


Install the C driver:

```powershell
# Tested with C driver 2.2.3.
cd C:\code\mongo-c-driver
cmake -S. -Bcmake-build -DENABLE_TESTS=OFF -DCMAKE_INSTALL_PREFIX="C:\install\mongo-c-driver"
cmake --build cmake-build --target INSTALL --config Debug
# Can use other values for --config
```

Install the C++ driver using the previously installed C driver. Use `CMAKE_CXX_STANDARD=17` to avoid the need for C++11 polyfills:
```powershell
# Tested with C++ drive r4.1.4.
cd C:\code\mongo-cxx-driver 
cmake -S. -Bcmake-build -DENABLE_TESTS=OFF -DCMAKE_INSTALL_PREFIX="C:\install\mongo-cxx-driver" -DCMAKE_PREFIX_PATH="C:\install\mongo-c-driver" -DCMAKE_CXX_STANDARD=17
cmake --build cmake-build --target INSTALL --config Debug
# Can use other values for --config
```

Build was tested with internet disconnected.