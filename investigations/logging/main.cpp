#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/logger.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>

class log_printer : public mongocxx::logger {
  void operator()(mongocxx::log_level level, bsoncxx::stdx::string_view domain,
                  bsoncxx::stdx::string_view message) noexcept override {
    std::cout << "log [" << domain << "] : " << message << std::endl;
  }
};

using namespace bsoncxx::builder::basic;
int main() {
  auto logger = std::make_unique<log_printer>();

  // Emit the informational mongocxx log message: "libmongoc logging callback enabled".
  mongocxx::instance instance{std::move(logger)};

  std::string uristr = "mongodb://localhost:27017";
  if (std::getenv("MONGODB_URI")) {
    uristr = std::getenv("MONGODB_URI");
  }

  auto client = mongocxx::client(mongocxx::uri(uristr));
  auto db = client.database("db");
  auto doc = make_document(kvp("ping", 1));
  auto res = db.run_command(doc.view());
  std::cout << "ping replied with " << bsoncxx::to_json(res.view()) << std::endl;
}
