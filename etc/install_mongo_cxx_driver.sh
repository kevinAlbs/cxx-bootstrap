set -o errexit
set -o xtrace

if [[ "$(basename $(pwd))" != "cxx-bootstrap" ]]; then
    echo "Error: run this script from cxx-bootstrap."
    exit 1
fi

MONGOCRYPT_PATH=${MONGOCRYPT_PATH:-$(pwd)/install/libmongocrypt-master}
MONGOC_PATH=${MONGOC_PATH:-$(pwd)/install/mongo-c-driver-master}
GITREF=${GITREF:-master}
PREFIX=${PREFIX:-$(pwd)/install/mongo-cxx-driver-$GITREF}
TMPDIR=$(pwd)/tmp

. ./etc/find_os.sh
. ./etc/find_cmake.sh

if [[ $OS == "WINDOWS" ]]; then
    MONGOCRYPT_PATH=$(cygpath -w $MONGOCRYPT_PATH)
    MONGOC_PATH=$(cygpath -w $MONGOC_PATH)
    PREFIX=$(cygpath -w $PREFIX)
fi

mkdir -p $PREFIX
rm -rf $TMPDIR
mkdir -p $TMPDIR

cd $TMPDIR
git clone git@github.com:mongodb/mongo-cxx-driver.git
cd mongo-cxx-driver
git checkout $GITREF
mkdir cmake-build
cd cmake-build

echo "About to install C++ driver ($GITREF) into $PREFIX"

$CMAKE \
    -DCMAKE_CXX_COMPILER_LAUNCHER="ccache" \
    -DCMAKE_CXX_STANDARD=17 \
    -DCMAKE_INSTALL_PREFIX=$PREFIX \
    -DENABLE_TESTS=OFF \
    -DCMAKE_PREFIX_PATH="$MONGOCRYPT_PATH;$MONGOC_PATH" ..

$CMAKE --build . --target install