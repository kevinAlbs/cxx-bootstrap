// Copyright 2017 MongoDB Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
#include <mongocxx/exception/query_exception.hpp>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

int main()
{
    mongocxx::instance inst{};

    mongocxx::uri uri{"mongodb://localhost:27017,localhost:27018,localhost:27019,localhost:27020,localhost:27021"};

    mongocxx::options::apm apm_opts;

    apm_opts.on_command_started(
        [](const mongocxx::events::command_started_event &event)
        {
            std::stringstream ss;
            ss << "[EVENT] command started " << event.command_name() << " to server " << event.host() << ":" << event.port() << std::endl;
            std::cout << ss.str() << std::flush;
        });

    apm_opts.on_command_succeeded(
        [](const mongocxx::events::command_succeeded_event &event)
        {
            std::stringstream ss;
            ss << "[EVENT] command succeeded " << event.command_name() << " to server " << event.host() << ":" << event.port() << std::endl;
            std::cout << ss.str() << std::flush;
        });

    apm_opts.on_command_failed(
        [](const mongocxx::events::command_failed_event &event)
        {
            std::stringstream ss;
            ss << "[EVENT] command failed " << event.command_name() << " to server " << event.host() << ":" << event.port() << " " << bsoncxx::to_json(event.failure()) << std::endl;
            std::cout << ss.str() << std::flush;
        });

    // Print heartbeat events:
    apm_opts.on_heartbeat_failed(
        [](const mongocxx::events::heartbeat_failed_event &event)
        {
            std::stringstream ss;
            ss << "[EVENT] heartbeat failed to " << event.host() << ":" << event.port() << " "
               << "with message '" << event.message() << "' and took " << (event.duration() / 1000) << "ms" << std::endl;
            std::cout
                << ss.str() << std::flush;
        });

    apm_opts.on_heartbeat_succeeded(
        [](const mongocxx::events::heartbeat_succeeded_event &event)
        {
            std::stringstream ss;
            ss << "[EVENT] heartbeat succeeded to " << event.host() << ":" << event.port() << " and took " << (event.duration() / 1000) << "ms" << std::endl;
            std::cout << ss.str() << std::flush;
        });

    mongocxx::options::client client_opts;
    client_opts.apm_opts(apm_opts);

    // Drop and create collection on separate client for repeatable results.
    {
        mongocxx::client client = mongocxx::client(uri);
        client["db"]["coll"].drop();
        client["db"]["coll"].insert_one(make_document(kvp("foo", "bar")));
    }

    // Run two finds. Pause in between to allow user to kill a mongod process.
    {
        mongocxx::client client = mongocxx::client(uri, client_opts);
        {
            std::cout << "Running find ..." << std::endl;
            auto start = std::chrono::system_clock::now();
            auto got = client["db"]["coll"].find_one(make_document());
            assert(got); // Expect a document is returned.
            std::cout << "Got document: " << bsoncxx::to_json(got->view()) << std::endl;
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            std::cout << "Running find ... done. Took " << elapsed_seconds.count() << "s" << std::endl;
        }

        std::cout << "Pausing. Run ./kill-one-mongod.sh to kill a mongod process. Press ENTER to run another find on the same client." << std::endl;
        std::string ignored;
        std::getline(std::cin, ignored);

        {
            std::cout << "Running find ..." << std::endl;
            auto start = std::chrono::system_clock::now();
            auto got = client["db"]["coll"].find_one(make_document());
            assert(got); // Expect a document is returned.
            std::cout << "Got document: " << bsoncxx::to_json(got->view()) << std::endl;
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            std::cout << "Running find ... done. Took " << elapsed_seconds.count() << "s" << std::endl;
        }
    }
}
