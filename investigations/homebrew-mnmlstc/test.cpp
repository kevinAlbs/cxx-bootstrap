#include <iostream>

#include <mongocxx/config/version.hpp>
#include <mongocxx/instance.hpp>

int main() {
    mongocxx::instance();
    std::cout << "Using C++ driver " << MONGOCXX_VERSION_STRING << std::endl;
    return 0;
}
