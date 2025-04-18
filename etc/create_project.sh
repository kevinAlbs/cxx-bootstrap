PROJECT_NAME=${PROJECT_NAME:-}

if [ -z "$PROJECT_NAME" ]; then
    PROJECT_NAME=${NAME:-}
fi

if [ -z "$PROJECT_NAME" ]; then
    echo "required environment variable PROJECT_NAME or NAME not supplied"
    exit 1
fi

if [[ -d "investigations/$PROJECT_NAME" ]]; then
    echo "investigations/$PROJECT_NAME already exists"
    exit 1
fi

mkdir -p investigations/$PROJECT_NAME
pushd investigations/$PROJECT_NAME

cat <<EOF > CMakeLists.txt
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project($PROJECT_NAME LANGUAGES C CXX)
find_package(mongocxx REQUIRED)
add_executable (main.out main.cpp)
target_link_libraries(main.out PRIVATE mongo::mongocxx_shared)
EOF

cat <<EOF > configure.sh
cmake \\
    -DCMAKE_PREFIX_PATH=../../install/mongo-cxx-driver-r4.0.0 \\
    -DCMAKE_BUILD_TYPE="Debug" \\
    -DCMAKE_C_FLAGS="-fsanitize=address" \\
    -DCMAKE_CXX_STANDARD=17 \\
    -S./ \\
    -B./cmake-build
EOF

chmod u+x configure.sh

cat <<EOF > build.sh
cmake --build cmake-build --target main.out
EOF

chmod u+x build.sh

cat <<EOF > run.sh
export DYLD_LIBRARY_PATH=$HOME/code/c-bootstrap/install/mongo-c-driver-1.30.0/lib/
./cmake-build/main.out
EOF

chmod u+x run.sh

cat <<EOF > main.cpp
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>

#include <cstdlib>
#include <iostream>

using namespace bsoncxx::builder::basic;
int main () {
    auto instance = mongocxx::instance();

    std::string uristr = "mongodb://localhost:27017";
    if (std::getenv ("MONGODB_URI")) {
        uristr = std::getenv ("MONGODB_URI");
    }

    auto client = mongocxx::client(mongocxx::uri(uristr));
    auto db = client.database("db");
    auto doc = make_document(kvp("ping", 1));
    auto res = db.run_command (doc.view());
    std::cout << "ping replied with " << bsoncxx::to_json (res.view()) << std::endl;
}
EOF

popd
