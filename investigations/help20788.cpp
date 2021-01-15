#include <cstdint>
#include <iostream>
#include <iomanip>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include "util.hpp"

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

int main() {
    std::cout << "Testing driver" << std::endl;

    mongocxx::instance instance{}; // This should be done only once.
    mongocxx::uri uri("mongodb://localhost:27017/?replicaSet=rs0&retryWrites=true&w=majority&serverSelectionTimeoutMS=300000");
    mongocxx::options::client opts;
    opts.apm_opts(get_apm_opts());
    mongocxx::client client(uri, opts);

    // Configure a failpoint.
    auto builder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value cmd = builder
        << "configureFailPoint" << "failCommand"
        << "mode" << bsoncxx::builder::stream::open_document << "times" << 1 << bsoncxx::builder::stream::close_document
        << "data" << bsoncxx::builder::stream::open_document
        << "failCommands" << bsoncxx::builder::stream::open_array << "insert" << bsoncxx::builder::stream::close_array
        << "errorCode" << 11602
        << "errorLabels" << bsoncxx::builder::stream::open_array << "RetryableWriteError" << bsoncxx::builder::stream::close_array
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::finalize;

    std::cout << "running failpoint cmd " << bsoncxx::to_json (cmd) << std::endl;
    client["admin"].run_command(cmd.view());
    std::cout << "ran failpoint cmd" << std::endl;

    mongocxx::database db = client["mydb"];
    mongocxx::collection coll = db["test"];

    builder.clear();
    bsoncxx::document::value doc_value = builder
        << "name" << "MongoDB"
        << "type" << "database"
        << "count" << 1
        << "versions" << bsoncxx::builder::stream::open_array
        << "v3.2" << "v3.0" << "v2.6"
        << close_array
        << "info" << bsoncxx::builder::stream::open_document
        << "x" << 203
        << "y" << 102
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::finalize;

    bsoncxx::document::view view = doc_value.view();

    bsoncxx::document::element element = view["name"];

    try {
        
        for (int i = 0; i < 3; i++) {
            auto result1 =
                coll.insert_one(view);
        }
    } catch (mongocxx::operation_exception e) {
        std::cout << "Error code: " << e.code() << std::endl;
        std::cout << "What string: " << e.what() << std::endl;
        std::cout << "Raw server error:" << std::endl;
        std::cout << bsoncxx::to_json(*e.raw_server_error()) << std::endl;
        throw e;
    }
}
