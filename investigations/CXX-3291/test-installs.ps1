$ErrorActionPreference="Stop"

# Test install behavior when passing /Z7 directly and using CMAKE_MSVC_DEBUG_INFORMATION_FORMAT.
# This script may take a long time to do installs.

if (Test-Path "C:/CXX-3291") {
    Remove-Item -Recurse "C:/CXX-3291" -Force
}

$SOURCE_DIR="C:/CXX-3291/mongo-cxx-driver"
git clone https://github.com/mongodb/mongo-cxx-driver $SOURCE_DIR

$BUILD_DIR="C:/CXX-3291/01-build"
$INSTALL_DIR="C:/CXX-3291/01-install"
$LOG_FILE="C:/CXX-3291/01-logs.txt"
git -C $SOURCE_DIR checkout --quiet r4.0.0
Write-Output "Installing r4.0.0 with /Z7 flags ..."
cmake -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" -DCMAKE_C_FLAGS_DEBUG="/Z7" -DCMAKE_CXX_FLAGS_DEBUG="/Z7" -DBUILD_SHARED_LIBS=OFF -S"$SOURCE_DIR" -B"$BUILD_DIR" -DBUILD_VERSION="4.0.0" | Tee-Object -FilePath $LOG_FILE
cmake --build "$BUILD_DIR" --target install --config Debug --verbose | Tee-Object -FilePath $LOG_FILE -Append
# Build output shows /Z7 applied to C and C++ sources.
Write-Output "Installing r4.0.0 with /Z7 flags ... installed to $INSTALL_DIR"

$BUILD_DIR="C:/CXX-3291/02-build"
$INSTALL_DIR="C:/CXX-3291/02-install"
$LOG_FILE="C:/CXX-3291/02-logs.txt"
git -C $SOURCE_DIR checkout --quiet r4.1.0
Write-Output "Installing r4.1.0 with /Z7 flags ..."
cmake -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" -DCMAKE_C_FLAGS_DEBUG="/Z7" -DCMAKE_CXX_FLAGS_DEBUG="/Z7" -DBUILD_SHARED_LIBS=OFF -S"$SOURCE_DIR" -B"$BUILD_DIR" -DBUILD_VERSION="4.1.0" | Tee-Object -FilePath $LOG_FILE
cmake --build "$BUILD_DIR" --target install --config Debug --verbose | Tee-Object -FilePath $LOG_FILE -Append
# Build output shows /ZI applied to C and C++ sources.
Write-Output "Installing r4.1.0 with /Z7 flags ... installed to $INSTALL_DIR"

$BUILD_DIR="C:/CXX-3291/03-build"
$INSTALL_DIR="C:/CXX-3291/03-install"
$LOG_FILE="C:/CXX-3291/03-logs.txt"
git -C $SOURCE_DIR checkout --quiet r4.1.0
Write-Output "Installing r4.1.0 with CMAKE_MSVC_DEBUG_INFORMATION_FORMAT=Embedded ..."
cmake -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" -DCMAKE_MSVC_DEBUG_INFORMATION_FORMAT=Embedded -DBUILD_SHARED_LIBS=OFF -S"$SOURCE_DIR" -B"$BUILD_DIR" -DBUILD_VERSION="4.1.0" | Tee-Object -FilePath $LOG_FILE
cmake --build "$BUILD_DIR" --target install --config Debug --verbose | Tee-Object -FilePath $LOG_FILE -Append
# Build output shows /Z7 applied to C and C++ sources.
Write-Output "Installing r4.1.0 with CMAKE_MSVC_DEBUG_INFORMATION_FORMAT=Embedded ... installed to $INSTALL_DIR"
