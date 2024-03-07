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

using bsoncxx::builder::basic::kvp;

int main()
{
    mongocxx::instance inst{};
    // Use a shorter serverSelectionTimeoutMS for faster exit when all servers are terminated.
    mongocxx::uri uri{"mongodb://"
                      "host.docker.internal:27017,host.docker.internal:27018,"
                      "host.docker.internal:27019/?serverSelectionTimeoutMS=3000"};

    mongocxx::options::apm apm_opts;
    // Print failed commands. This prints failed commands, even if a retry succeeds.
    apm_opts.on_command_failed(
        [](const mongocxx::events::command_failed_event &event)
        {
            std::cout << "command failed " << event.command_name() << " "
                      << bsoncxx::to_json(event.failure()) << std::endl;
        });

    // Print server change events. This prints when a server is marked Unknown.
    apm_opts.on_server_changed(
        [](const mongocxx::events::server_changed_event &event)
        {
            std::cout << "server changed " << event.host() << ":" << event.port()
                      << " changed from " << event.previous_description().type() << " to "
                      << event.new_description().type() << std::endl
                      << "most recent description "
                      << bsoncxx::to_json(event.new_description().hello()) << std::endl;
        });

    // Print server heartbeat failures.
    apm_opts.on_heartbeat_failed(
        [](const mongocxx::events::heartbeat_failed_event &event)
        {
            std::cout << "heartbeat failed to " << event.host() << ":" << event.port()
                      << "with message " << event.message() << std::endl;
        });

    mongocxx::options::client client_opts;
    client_opts.apm_opts(apm_opts);
    mongocxx::pool pool{uri, mongocxx::options::pool{client_opts}};
    std::vector<std::string> collection_names = {"countries", "bar", "baz"};
    std::vector<std::thread> threads{};

    for (auto i : {0, 1, 2})
    {
        auto run = [&](std::int64_t j)
        {
            // Each client and collection can only be used in a single thread.
            auto client = pool.acquire();
            auto coll = (*client)["countries"][collection_names[j]];

            bsoncxx::types::b_int64 index = {j};
            coll.insert_one(bsoncxx::builder::basic::make_document(kvp("x", index)));

            std::cout << "Thread " << j << " about to run `find_one` in a loop" << std::endl;
            auto prevTime = std::chrono::system_clock::now();
            while (true)
            {
                auto doc = (*client)["countries"][collection_names[j]].find_one({});
                if (!doc)
                {
                    std::cout << "Unexpected: no document found" << std::endl;
                }

                auto currentTime = std::chrono::system_clock::now();
                auto dur = std::chrono::duration_cast<std::chrono::seconds>(currentTime - prevTime);
                if (dur.count() > 1)
                {
                    std::cout << "Thread " << j << " still running " << std::endl;
                    prevTime = currentTime;
                }
            }

            // The client goes out of scope at the end of the lambda and is returned to the pool.
        };

        std::thread runner{run, i};

        threads.push_back(std::move(runner));
    }

    for (auto &&runner : threads)
    {
        runner.join();
    }
}
