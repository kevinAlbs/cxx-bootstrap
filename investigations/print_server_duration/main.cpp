#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <cstdlib>
#include <iostream>

using namespace bsoncxx::builder::basic;
int main() {
  auto instance = mongocxx::instance();

  auto client_opts = mongocxx::options::client();
  auto apm_opts = mongocxx::options::apm();

  apm_opts.on_command_succeeded([](const mongocxx::events::command_succeeded_event &event) {
    std::cout << event.command_name() << " succeeded with duration: " << event.duration() << " microseconds"
              << std::endl;
  });

  apm_opts.on_command_failed([](const mongocxx::events::command_failed_event &event) {
    std::cout << event.command_name() << " failed with duration: " << event.duration() << " microseconds" << std::endl;
  });

  client_opts.apm_opts(apm_opts);

  auto client = mongocxx::client(mongocxx::uri("mongodb://localhost:27017"), client_opts);
  auto db = client.database("db");
  auto doc = make_document(kvp("ping", 1));
  auto res = db.run_command(doc.view());
  std::cout << "ping replied with " << bsoncxx::to_json(res.view()) << std::endl;
}
