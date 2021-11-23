set -o errexit
set -o xtrace

if [[ "$(basename $(pwd))" != "cxx-bootstrap" ]]; then
    echo "Error: run this script from cxx-bootstrap."
    exit 1
fi

./etc/install_libbson.sh
./etc/install_libmongocrypt.sh
./etc/install_mongo_c_driver.sh
./etc/install_mongo_cxx_driver.sh