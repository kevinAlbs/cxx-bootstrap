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

  std::string uristr = "mongodb://localhost:27017";
  if (std::getenv("MONGODB_URI")) {
    uristr = std::getenv("MONGODB_URI");
  }

  auto client_opts = mongocxx::options::client();
  auto apm_opts = mongocxx::options::apm();

  apm_opts.on_server_changed([](const mongocxx::events::server_changed_event &event) {
    auto new_desc = event.new_description();
    auto old_desc = event.previous_description();

    std::cout << "server changed " << event.host() << ":" << event.port() << std::endl;
    std::cout << "    old: " << old_desc.type() << " "
              << bsoncxx::to_json(old_desc.hello(), bsoncxx::ExtendedJsonMode::k_relaxed) << std::endl;
    std::cout << "    new: " << new_desc.type() << " "
              << bsoncxx::to_json(new_desc.hello(), bsoncxx::ExtendedJsonMode::k_relaxed) << std::endl;
  });

  client_opts.apm_opts(apm_opts);

  auto client = mongocxx::client(mongocxx::uri(uristr), client_opts);
  auto db = client.database("db");
  auto doc = make_document(kvp("ping", 1));
  auto res = db.run_command(doc.view());
  std::cout << "ping replied with " << bsoncxx::to_json(res.view()) << std::endl;
}
