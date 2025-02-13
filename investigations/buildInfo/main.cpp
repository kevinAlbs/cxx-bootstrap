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
  auto client = mongocxx::client(mongocxx::uri("mongodb://localhost:27017"));
  auto db = client.database("db");
  auto cmd = make_document(kvp("buildInfo", 1));
  auto res = db.run_command(cmd.view());
  std::cout << "got version: " << res["version"].get_string().value << std::endl;
}
