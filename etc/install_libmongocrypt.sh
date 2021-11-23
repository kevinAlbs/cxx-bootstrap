set -o errexit
set -o xtrace

if [[ "$(basename $(pwd))" != "cxx-bootstrap" ]]; then
    echo "Error: run this script from cxx-bootstrap."
    exit 1
fi

LIBBSON_PATH=${LIBBSON_PATH:-$(pwd)/install/libbson-master}
GITREF=${GITREF:-master}
PREFIX=${PREFIX:-$(pwd)/install/libmongocrypt-$GITREF}
TMPDIR=$(pwd)/tmp

. ./etc/find_os.sh
. ./etc/find_cmake.sh

if [[ $OS == "WINDOWS" ]]; then
    LIBBSON_PATH=$(cygpath -w $LIBBSON_PATH)
    PREFIX=$(cygpath -w $PREFIX)
fi

mkdir -p $PREFIX
rm -rf $TMPDIR
mkdir -p $TMPDIR

cd $TMPDIR
git clone git@github.com:mongodb/libmongocrypt.git
cd libmongocrypt
git checkout $GITREF
mkdir cmake-build
cd cmake-build

echo "About to install libmongocrypt ($GITREF) into $PREFIX"

$CMAKE -DCMAKE_INSTALL_PREFIX=$PREFIX \
    -DCMAKE_PREFIX_PATH=$LIBBSON_PATH \
    -DCMAKE_C_COMPILER="/Users/kevin.albertson/bin/llvm-11.0.0/bin/clang" \
    -DCMAKE_C_COMPILER_LAUNCHER="ccache" ..

$CMAKE --build . --target install