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

  auto client = mongocxx::client(mongocxx::uri(uristr));
  auto db = client.database("db");
  auto coll = db.collection("coll");

  // Start session to capture operation timestamp.
  auto sess = client.start_session();

  coll.insert_one(sess, make_document(kvp("foo", "bar")));

  // Capture operation time.
  auto ts = sess.operation_time();

  // Insert with majority write concern. Change streams use majority read concern.
  {
    auto wc_majority = mongocxx::write_concern();
    wc_majority.acknowledge_level(mongocxx::write_concern::level::k_majority);
    auto i_opts = mongocxx::options::insert();
    i_opts.write_concern(wc_majority);
    coll.insert_one(sess, make_document(kvp("foo", "bar")), i_opts);
  }

  auto cs_opts = mongocxx::options::change_stream();
  cs_opts.start_at_operation_time(ts);

  auto cs = coll.watch(sess, cs_opts);
  for (auto change : cs) {
    std::cout << "got change " << bsoncxx::to_json(change) << std::endl;
  }
}
