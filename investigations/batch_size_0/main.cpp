// Copyright 2018-present MongoDB Inc.
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

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <mongocxx/change_stream.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

// watch_forever iterates the change stream until an error occurs.
void watch_forever(mongocxx::collection &collection) {
  mongocxx::options::change_stream options;
  options.batch_size(0);
  mongocxx::change_stream stream = collection.watch(options); // Does not send batchSize for 'aggregate or 'getMore'.

  while (true) {
    for (const auto &event : stream) {
      std::cout << bsoncxx::to_json(event) << std::endl;
    }
    std::cout << "No new notifications. Trying again..." << std::endl;
  }
}

int main(int argc, char *argv[]) {
  mongocxx::instance inst{};
  auto uri_str = mongocxx::uri::k_default_uri;
  std::string db = "db";
  std::string coll = "coll";

  auto apm_opts = mongocxx::options::apm().on_command_started([](const mongocxx::events::command_started_event &event) {
    std::cout << "-> command started " << event.command_name() << " " << event.database_name() << " "
              << bsoncxx::to_json(event.command()) << std::endl;
  });
  auto opts = mongocxx::options::client().apm_opts(apm_opts);
  mongocxx::pool pool{mongocxx::uri(uri_str), opts};

  try {
    auto entry = pool.acquire();
    auto collection = (*entry)[db][coll];

    std::cout << "Watching for notifications on the collection " << db << "." << coll << std::endl;
    std::cout << "To observe a notification, try inserting a document." << std::endl;
    watch_forever(collection);

    return EXIT_SUCCESS;
  } catch (const std::exception &exception) {
    std::cerr << "Caught exception \"" << exception.what() << "\"" << std::endl;
  } catch (...) {
    std::cerr << "Caught unknown exception type" << std::endl;
  }

  return EXIT_FAILURE;
}
