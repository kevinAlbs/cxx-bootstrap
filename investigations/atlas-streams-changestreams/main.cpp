#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <cstdlib>
#include <iostream>

// get_apm_opts returns Application Performance Monitoring (APM) options:
static inline mongocxx::options::apm get_apm_opts(void) {
  mongocxx::options::apm opts;

  opts.on_command_started([](const mongocxx::events::command_started_event &event) {
    std::cout << "-> command started " << event.command_name() << " " << event.database_name() << " "
              << bsoncxx::to_json(event.command()) << std::endl;
  });

  opts.on_command_succeeded([](const mongocxx::events::command_succeeded_event &event) {
    std::cout << "<- command succeeded " << event.command_name() << " " << bsoncxx::to_json(event.reply()) << std::endl;
  });

  opts.on_command_failed([](const mongocxx::events::command_failed_event &event) {
    std::cout << "<- command failed " << event.command_name() << " " << bsoncxx::to_json(event.failure()) << std::endl;
  });

  return opts;
}

int main() {
  auto instance = mongocxx::instance();

  std::string uristr = "mongodb://localhost:27017";
  auto client_opts = mongocxx::options::client();
  client_opts.apm_opts(get_apm_opts());
  auto client = mongocxx::client(mongocxx::uri(uristr), client_opts);
  auto cs = client["db"]["coll"].watch(); // sends aggregate.
  auto cs_iter = cs.begin();
  while (true) {
    if (cs_iter == cs.end()) {
      std::cout << "No event yet." << std::endl;
      cs_iter = cs.begin(); // sends getMore.
    } else {
      auto event = *cs_iter;
      std::cout << "Got event: " << bsoncxx::to_json(event) << std::endl;
      cs_iter++; // only sends getMore if no events batched
    }
  }
}
