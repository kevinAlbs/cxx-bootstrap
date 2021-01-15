#include <mongocxx/client.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>
#include "util.hpp"

// Try to read an int32 from "view".
void run_test (bsoncxx::document::view view, std::string key) {
    try {
        std::cout << "Reading " << key << " from document" << bsoncxx::to_json (view) << std::endl;
        int32_t value = view[key].get_int32().value;
    } catch (bsoncxx::exception& e) {
        if (e.code() == bsoncxx::error_code::k_unset_element) {
            std::cout << " k_unset_element exception thrown" << std::endl;
        } else if (e.code() == bsoncxx::error_code::k_need_element_type_k_int32) {
            std::cout << " k_need_element_type_k_int32 exception thrown" << std::endl;
        } else {
            std::cout << " unexpected exception thrown" << std::endl;
        }
    }
}

int main () {
    auto doc = bsoncxx::builder::basic::document{};
    using bsoncxx::builder::basic::kvp;

    doc.append(kvp("foo", "bar"));
    run_test (doc, "foo");
    run_test (doc, "unknown");
}