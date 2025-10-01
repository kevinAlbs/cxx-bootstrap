/*
Show an example of printing a server changed event. Example:

ping ... begin
server changed localhost:27017
    old: Unknown { }
    new: RSPrimary { "helloOk" : true, "topologyVersion" : { "processId" : { "$oid" : "68da75a0b983a74e5a236920" },
"counter" : 61 }, "hosts" : [ "localhost:27017" ], "passives" : [ "localhost:27018" ], "setName" : "oidc-repl0",
"setVersion" : 1, "ismaster" : true, "secondary" : false, "primary" : "localhost:27017", "me" : "localhost:27017",
"electionId" : { "$oid" : "7fffffff000000000000000d" }, "lastWrite" : { "opTime" : { "ts" : { "$timestamp" : { "t" :
1759320544, "i" : 1 } }, "t" : 13 }, "lastWriteDate" : { "$date" : "2025-10-01T12:09:04Z" }, "majorityOpTime" : { "ts" :
{ "$timestamp" : { "t" : 1759320544, "i" : 1 } }, "t" : 13 }, "majorityWriteDate" : { "$date" : "2025-10-01T12:09:04Z" }
}, "maxBsonObjectSize" : 16777216, "maxMessageSizeBytes" : 48000000, "maxWriteBatchSize" : 100000, "localTime" : {
"$date" : "2025-10-01T12:09:04.652Z" }, "logicalSessionTimeoutMinutes" : 30, "connectionId" : 760, "minWireVersion" : 0,
"maxWireVersion" : 28, "readOnly" : false, "ok" : 1.0, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" :
1759320544, "i" : 1 } }, "signature" : { "hash" : { "$binary" : { "base64" : "lbev6+1+fmJXvHadj/E2CCBwf48=", "subType" :
"00" } }, "keyId" : 7555480633447809029 } }, "operationTime" : { "$timestamp" : { "t" : 1759320544, "i" : 1 } } } ping
... end

*/
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
  std::cout << "ping ... begin" << std::endl;
  auto res = db.run_command(doc.view());
  std::cout << "ping ... end" << std::endl;
}
