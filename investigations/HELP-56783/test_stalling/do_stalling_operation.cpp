#include <iostream>
#include <ostream>
#include <sstream>
#include <thread>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

void do_stalling_operation(mongocxx::pool &pool)
{
    auto client = pool.acquire();
    auto db = (*client)["db"];
    db["coll"].drop();
    db["coll"].insert_one(make_document(kvp("foo", "bar")));
    auto cmd = R"(
        {
            "aggregate": "coll",
            "pipeline": [
                {
                    "$match": {"foo": "bar"}
                },
                {
                    "$project": {
                    "output": {
                        "$function": {
                        "body": "function() { sleep(1000 * 30); return \"foo\"; }",
                        "args": [],
                        "lang": "js"
                        }
                    }
                    }
                }
            ],
            "cursor": {}
        }
    )";
    auto cmd_bson = bsoncxx::from_json(cmd);
    std::cout << "Running stalling operation ... begin" << std::endl;
    auto reply = db.run_command(cmd_bson.view());
    std::cout << "Running stalling operation ... end" << std::endl;
    std::cout << "got reply " << bsoncxx::to_json(reply) << std::endl;
}

int main()
{
    mongocxx::instance inst{};
    const char *uri_str = getenv("MONGODB_URI");
    if (uri_str == nullptr)
    {
        // Assume process is running in docker.
        uri_str = "mongodb://"
                  "host.docker.internal:27017,host.docker.internal:27018,"
                  "host.docker.internal:27019/?serverSelectionTimeoutMS=3000";
        return 1;
    }
    // Use a shorter serverSelectionTimeoutMS for faster exit when all servers are terminated.
    mongocxx::uri uri{uri_str};

    mongocxx::options::apm apm_opts;

    // Print failed commands. This prints failed commands, even if a retry succeeds.
    apm_opts.on_command_succeeded(
        [](const mongocxx::events::command_succeeded_event &event)
        {
            if (event.command_name() != "find")
            {
                std::stringstream ss;
                ss << "command succeeded " << event.command_name() << std::endl;
                std::cout << ss.str() << std::flush;
            }
        });

    // Print failed commands. This prints failed commands, even if a retry succeeds.
    apm_opts.on_command_failed(
        [](const mongocxx::events::command_failed_event &event)
        {
            std::stringstream ss;
            ss << "command failed " << event.command_name() << " "
               << bsoncxx::to_json(event.failure()) << std::endl;
            std::cout << ss.str() << std::flush;
        });

    // Print server change events. This prints when a server is marked Unknown.
    apm_opts.on_server_changed(
        [](const mongocxx::events::server_changed_event &event)
        {
            std::stringstream ss;
            ss << "server changed " << event.host() << ":" << event.port()
               << " changed from " << event.previous_description().type() << " to "
               << event.new_description().type() << std::endl
               << "most recent description "
               << bsoncxx::to_json(event.new_description().hello()) << std::endl;
            std::cout << ss.str() << std::flush;
        });

    // Print server heartbeat failures.
    apm_opts.on_heartbeat_failed(
        [](const mongocxx::events::heartbeat_failed_event &event)
        {
            std::stringstream ss;
            ss << "heartbeat failed to " << event.host() << ":" << event.port() << " "
               << "with message " << event.message() << std::endl;
            std::cout << ss.str() << std::flush;
        });

    mongocxx::options::client client_opts;
    client_opts.apm_opts(apm_opts);
    mongocxx::pool pool{uri, mongocxx::options::pool{client_opts}};

    do_stalling_operation(pool);
}
