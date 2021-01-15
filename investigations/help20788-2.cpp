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
#include <thread>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

// Start a two node replica set.
#define CONNECTION_STRING "mongodb://localhost:27017,localhost:27018/?replicaSet=rs0&retryWrites=true&w=majority&serverSelectionTimeoutMS=300000&serverSelectionTryOnce=false"

mongocxx::options::apm get_apm_opts(void)
{
    mongocxx::options::apm opts;

    opts.on_command_started([](const mongocxx::events::command_started_event &event) {
        std::cout << "-> command started " << event.command_name() << " " << event.database_name() << " " << bsoncxx::to_json(event.command()) << std::endl;
    });

    opts.on_command_succeeded([](const mongocxx::events::command_succeeded_event &event) {
        std::cout << "<- command succeeded " << event.command_name() << " " << bsoncxx::to_json(event.reply()) << std::endl;
    });

    opts.on_command_failed([](const mongocxx::events::command_failed_event &event) {
        std::cout << "<- command failed " << event.command_name() << " " << bsoncxx::to_json(event.failure()) << std::endl;
    });

    opts.on_server_changed([](const mongocxx::events::server_changed_event &event) {
        std::cout << "server changed " << event.host() << ":" << event.port() << " changed from " << event.previous_description().type() << " to " << event.new_description().type() << std::endl;
        std::cout << "most recent ismaster " << bsoncxx::to_json(event.new_description().is_master()) << std::endl;
    });

    return opts;
}

void stepdown(void)
{
    std::cout << "beginning stepdown" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    mongocxx::uri uri(CONNECTION_STRING);
    mongocxx::options::client opts;
    mongocxx::client client(uri, opts);

    auto cmd = bsoncxx::builder::basic::make_document(kvp("replSetStepDown", 30));
    client["admin"].run_command (cmd.view());
    std::cout << "ending stepdown" << std::endl;
}

int main()
{
    std::cout << "Testing driver" << std::endl;

    mongocxx::instance instance{}; // This should be done only once.
    mongocxx::uri uri(CONNECTION_STRING);
    mongocxx::options::client opts;
    opts.apm_opts(get_apm_opts());
    mongocxx::pool pool (uri);
    //mongocxx::client client(uri, opts);
    auto entry = pool.acquire();
    mongocxx::client& client = *entry;

    mongocxx::database db = client["mydb"];
    mongocxx::collection coll = db["test"];

    std::thread t (stepdown);

    auto builder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value doc_value = builder
                                         << "name"
                                         << "MongoDB"
                                         << "type"
                                         << "database"
                                         << "count" << 1
                                         << "versions" << bsoncxx::builder::stream::open_array
                                         << "v3.2"
                                         << "v3.0"
                                         << "v2.6"
                                         << close_array
                                         << "info" << bsoncxx::builder::stream::open_document
                                         << "x" << 203
                                         << "y" << 102
                                         << bsoncxx::builder::stream::close_document
                                         << bsoncxx::builder::stream::finalize;

    bsoncxx::document::view view = doc_value.view();

    bsoncxx::document::element element = view["name"];

    try
    {
        while (true)
        {
            auto result1 =
                coll.insert_one(view);
        }
    }
    catch (mongocxx::operation_exception e)
    {
        std::cout << "Error code: " << e.code() << std::endl;
        std::cout << "What string: " << e.what() << std::endl;
        std::cout << "Raw server error:" << std::endl;
        std::cout << bsoncxx::to_json(*e.raw_server_error()) << std::endl;
        throw e;
    }
}
