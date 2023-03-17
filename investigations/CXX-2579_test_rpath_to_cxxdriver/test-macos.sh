#!/bin/bash 

CXX_INSTALL_PATH=/Users/kevin.albertson/code/cxx-bootstrap/install/mongo-cxx-driver-r3.7.0

function assert_eq () {
    a="$1"
    b="$2"
    if [[ "$a" != "$b" ]]; then
        echo "Assertion failed: $a != $b"
        # Print caller
        caller
        exit 1
    fi
}

function assert_contains ()   {
    a="$1"
    b="$2"
    if [[ "$a" != *"$b"* ]]; then
        echo "Assertion failed: '$a' does not contain '$b'"
        # Print caller
        caller
        exit 1
    fi
}

function assert_not_contains ()   {
    a="$1"
    b="$2"
    if [[ "$a" == *"$b"* ]]; then
        echo "Assertion failed: '$a' does contain '$b'"
        # Print caller
        caller
        exit 1
    fi
}

function cleanup () {
    (
        rm app.out || true
        rm -rf cmake-build || true
    ) > /dev/null 2>&1
}

# Test getting "Library not loaded".
{
    cleanup
    captured=$((
        # Tell pkg-config where to find C++ driver installation.
        export PKG_CONFIG_PATH=$CXX_INSTALL_PATH/lib/pkgconfig
        clang++ -std=c++17 $(pkg-config --cflags --libs libmongocxx) -o ./app.out app.cpp
        ./app.out
        # Prints the following error:
        # dyld[3217]: Library not loaded: '@rpath/libmongocxx._noabi.dylib'
        #   Referenced from: '/Users/kevin.albertson/code/app.out'
        #   Reason: tried: '/usr/local/lib/libmongocxx._noabi.dylib' (no such file), '/usr/lib/libmongocxx._noabi.dylib' (no such file)
        # zsh: abort      ./app.out
    )2>&1)
    assert_contains "$captured" "Library not loaded"
}

# Test DYLD_FALLBACK_LIBRARY_PATH
{
    cleanup
    # Tell pkg-config where to find C++ driver installation.
    export PKG_CONFIG_PATH=$CXX_INSTALL_PATH/lib/pkgconfig
    clang++ -std=c++17 $(pkg-config --cflags --libs libmongocxx) -o ./app.out app.cpp
    captured=$((
        DYLD_FALLBACK_LIBRARY_PATH=$CXX_INSTALL_PATH/lib ./app.out
        # Prints "successfully connected with C++ driver"
    )2>&1)
    assert_contains "$captured" "success"
}

# Test -Wl,-rpath
{
    cleanup
    captured=$((
        # Tell pkg-config where to find C++ driver installation.
        export PKG_CONFIG_PATH=$CXX_INSTALL_PATH/lib/pkgconfig
        # Pass the linker option -rpath to set an rpath in the final executable.
        clang++ -std=c++17 -Wl,-rpath,$CXX_INSTALL_PATH/lib $(pkg-config --cflags --libs libmongocxx) -o ./app.out app.cpp
        ./app.out
        # Prints "successfully connected with C++ driver"
    )2>&1)
    assert_contains "$captured" "success"
}

# Test install_name_tool
{
    cleanup
    captured=$((
        # Tell pkg-config where to find C++ driver installation.
        export PKG_CONFIG_PATH=$CXX_INSTALL_PATH/lib/pkgconfig
        clang++ -std=c++17 $(pkg-config --cflags --libs libmongocxx) -o ./app.out app.cpp
        # Add rpath entry.
        install_name_tool -add_rpath $CXX_INSTALL_PATH/lib app.out
        ./app.out
        # Prints "successfully connected with C++ driver"
    )2>&1)
    assert_contains "$captured" "success"
}

# Test cmake install getting "Library not loaded"
{
    cleanup
    captured=$((
        # Build application `app` using the C++ driver from a non-standard install.
        cmake \
        -DCMAKE_PREFIX_PATH=$CXX_INSTALL_PATH \
        -DCMAKE_INSTALL_PREFIX=$HOME/app \
        -DCMAKE_CXX_STANDARD=17 \
        -Bcmake-build -S.
        cmake --build cmake-build --target app.out
        # Running app.out from build tree includes rpath to C++ driver.
        ./cmake-build ./cmake-build/app.out # Prints: "successfully connected with C++ driver"

        cmake --build cmake-build --target install
        # Running app.out from install tree does not include rpath to C++ driver.
        $HOME/app/bin/app.out
        # Prints "Library not loaded" error.
    )2>&1)
    assert_contains "$captured" "Library not loaded"
}

{
    cleanup
    captured=$((
        # Build application `app` using the C++ driver from a non-standard install.
        # Use CMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE to keep rpath entry on installed app.
        cmake \
        -DCMAKE_PREFIX_PATH=$CXX_INSTALL_PATH \
        -DCMAKE_INSTALL_PREFIX=$HOME/app \
        -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE \
        -DCMAKE_CXX_STANDARD=17 \
        -Bcmake-build -S.

        cmake --build cmake-build --target install
        $HOME/app/bin/app.out
        # Prints "successfully connected with C++ driver"
    )2>&1)
    assert_contains "$captured" "success"
}

echo "Tests passed!"