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
#include <mutex>

int main(int argc, char *argv[]) {
  mongocxx::instance inst{};
  auto uri_str = mongocxx::uri::k_default_uri;
  std::string db = "db";
  std::string coll = "coll";
  int64_t last_cursor_id = 0;

  auto apm_opts =
      mongocxx::options::apm().on_command_succeeded([&](const mongocxx::events::command_succeeded_event &event) {
        if (event.command_name() != "aggregate") {
          return;
        }
        // Capture the cursor ID.
        last_cursor_id = event.reply()["cursor"]["id"].get_int64();
      });
  auto opts = mongocxx::options::client().apm_opts(apm_opts);
  mongocxx::client client{mongocxx::uri(uri_str), opts};

  try {
    auto collection = client[db][coll];
    std::cout << "Watching for notifications on the collection " << db << "." << coll << std::endl;
    std::cout << "To observe a notification, try inserting a document." << std::endl;

    // Iterate the change stream forever.
    {
      mongocxx::change_stream stream = collection.watch();
      while (true) {
        for (const auto &event : stream) {
          std::cout << bsoncxx::to_json(event) << std::endl;
        }
        std::cout << "Cursor id: " << last_cursor_id << std::endl;
        std::cout << "No new notifications. Trying again..." << std::endl;
      }
    }

    return EXIT_SUCCESS;
  } catch (const std::exception &exception) {
    std::cerr << "Caught exception \"" << exception.what() << "\"" << std::endl;
  } catch (...) {
    std::cerr << "Caught unknown exception type" << std::endl;
  }

  return EXIT_FAILURE;
}
