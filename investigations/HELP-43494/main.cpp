#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/json.hpp>

#include <cstdlib>
#include <iostream>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

// JSON_STR stringifies arguments.
#define JSON_STR(...) #__VA_ARGS__

// enable_failpoint_insert enables a failpoint on 'insert' to return a retryable write error.
// Requires mongod be started with `--setParameter enableTestCommands=1`
static void enable_failpoint_insert(mongocxx::client &client)
{
    const char *failpoint_insert_str = JSON_STR({
        "configureFailPoint" : "failCommand",
        "mode" : {
            "times" : 1
        },
        "data" : {
            "failCommands" : ["insert"],
            "errorCode" : 112,
            "errorLabels" : ["RetryableWriteError"]
        }
    });
    auto failpoint_insert_bson = bsoncxx::from_json(failpoint_insert_str);
    client["admin"].run_command(failpoint_insert_bson.view());
}

// enable_failpoint_find enables a failpoint on 'find' to return a retryable read error.
// Requires mongod be started with `--setParameter enableTestCommands=1`
static void enable_failpoint_find(mongocxx::client &client)
{
    const char *failpoint_find_str = JSON_STR({
        "configureFailPoint" : "failCommand",
        "mode" : {
            "times" : 1
        },
        "data" : {
            "failCommands" : ["find"],
            "errorCode" : 11600
        }
    });
    auto failpoint_find_bson = bsoncxx::from_json(failpoint_find_str);
    client["admin"].run_command(failpoint_find_bson.view());
}

static void command_started_cb(const mongocxx::events::command_started_event &event)
{
    std::cout << "-> command started " << event.command_name() << std::endl;
    std::cout << "  database_name " << event.database_name() << std::endl;
    std::cout << "  command " << bsoncxx::to_json(event.command()) << std::endl;
    std::cout << "  operation_id " << event.operation_id() << std::endl;
    std::cout << "  request_id " << event.request_id() << std::endl;
}

static void command_succeeded_cb(const mongocxx::events::command_succeeded_event &event)
{
    std::cout << "<- command succeeded " << event.command_name() << std::endl;
    std::cout << "  reply " << bsoncxx::to_json(event.reply()) << std::endl;
    std::cout << "  operation_id " << event.operation_id() << std::endl;
    std::cout << "  request_id " << event.request_id() << std::endl;
}

static void command_failed_cb(const mongocxx::events::command_failed_event &event)
{
    std::cout << "<- command failed " << event.command_name() << std::endl;
    std::cout << "  failure " << bsoncxx::to_json(event.failure()) << std::endl;
    std::cout << "  operation_id " << event.operation_id() << std::endl;
    std::cout << "  request_id " << event.request_id() << std::endl;
}

int main()
{
    auto instance = mongocxx::instance();

    std::string uristr = "mongodb://localhost:27017";
    if (std::getenv("MONGODB_URI"))
    {
        uristr = std::getenv("MONGODB_URI");
    }

    auto client_opts = mongocxx::options::client();
    auto apm_opts = mongocxx::options::apm();

    apm_opts.on_command_started(command_started_cb);
    apm_opts.on_command_succeeded(command_succeeded_cb);
    apm_opts.on_command_failed(command_failed_cb);

    client_opts.apm_opts(apm_opts);

    auto client = mongocxx::client(mongocxx::uri(uristr), client_opts);
    enable_failpoint_insert(client);
    std::cout << "insert ... begin" << std::endl;
    client["db"]["coll"].insert_one(make_document(kvp("foo", "bar")));
    std::cout << "insert ... end" << std::endl;
    enable_failpoint_find(client);
    std::cout << "find ... begin" << std::endl;
    client["db"]["coll"].find_one(make_document());
    std::cout << "find ... end" << std::endl;
}

/*
Sample output:
-> command started configureFailPoint
  database_name admin
  command { "configureFailPoint" : "failCommand", "mode" : { "times" : 1 }, "data" : { "failCommands" : [ "insert" ], "errorCode" : 112, "errorLabels" : [ "RetryableWriteError" ] }, "$db" : "admin", "$readPreference" : { "mode" : "primaryPreferred" }, "lsid" : { "id" : { "$binary" : "5vwfhDpzQGSCzWCoMxjRIA==", "$type" : "04" } }, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1679060278, "i" : 165 } }, "signature" : { "hash" : { "$binary" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "$type" : "00" }, "keyId" : 0 } } }
  operation_id 16808
  request_id 1
<- command succeeded configureFailPoint
  reply { "count" : 22, "ok" : 1.0, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1679060278, "i" : 165 } }, "signature" : { "hash" : { "$binary" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "$type" : "00" }, "keyId" : 0 } }, "operationTime" : { "$timestamp" : { "t" : 1679060278, "i" : 165 } } }
  operation_id 16808
  request_id 1
insert ... begin
-> command started insert
  database_name db
  command { "insert" : "coll", "ordered" : true, "$db" : "db", "lsid" : { "id" : { "$binary" : "5vwfhDpzQGSCzWCoMxjRIA==", "$type" : "04" } }, "txnNumber" : 1, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1679060278, "i" : 165 } }, "signature" : { "hash" : { "$binary" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "$type" : "00" }, "keyId" : 0 } }, "documents" : [ { "_id" : { "$oid" : "64146d3613d8a5029d057dc1" }, "foo" : "bar" } ] }
  operation_id 16809
  request_id 3
<- command failed insert
  failure { "errorLabels" : [ "RetryableWriteError" ], "ok" : 0.0, "errmsg" : "Failing command via 'failCommand' failpoint", "code" : 112, "codeName" : "WriteConflict", "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1679060278, "i" : 165 } }, "signature" : { "hash" : { "$binary" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "$type" : "00" }, "keyId" : 0 } }, "operationTime" : { "$timestamp" : { "t" : 1679060278, "i" : 165 } } }
  operation_id 16809
  request_id 3
-> command started insert
  database_name db
  command { "insert" : "coll", "ordered" : true, "$db" : "db", "lsid" : { "id" : { "$binary" : "5vwfhDpzQGSCzWCoMxjRIA==", "$type" : "04" } }, "txnNumber" : 1, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1679060278, "i" : 165 } }, "signature" : { "hash" : { "$binary" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "$type" : "00" }, "keyId" : 0 } }, "documents" : [ { "_id" : { "$oid" : "64146d3613d8a5029d057dc1" }, "foo" : "bar" } ] }
  operation_id 16809
  request_id 5
<- command succeeded insert
  reply { "n" : 1, "electionId" : { "$oid" : "7fffffff0000000000000005" }, "opTime" : { "ts" : { "$timestamp" : { "t" : 1679060278, "i" : 167 } }, "t" : 5 }, "ok" : 1.0, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1679060278, "i" : 167 } }, "signature" : { "hash" : { "$binary" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "$type" : "00" }, "keyId" : 0 } }, "operationTime" : { "$timestamp" : { "t" : 1679060278, "i" : 167 } } }
  operation_id 16809
  request_id 5
insert ... end
-> command started configureFailPoint
  database_name admin
  command { "configureFailPoint" : "failCommand", "mode" : { "times" : 1 }, "data" : { "failCommands" : [ "find" ], "errorCode" : 11600 }, "$db" : "admin", "$readPreference" : { "mode" : "primaryPreferred" }, "lsid" : { "id" : { "$binary" : "5vwfhDpzQGSCzWCoMxjRIA==", "$type" : "04" } }, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1679060278, "i" : 167 } }, "signature" : { "hash" : { "$binary" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "$type" : "00" }, "keyId" : 0 } } }
  operation_id 16810
  request_id 7
<- command succeeded configureFailPoint
  reply { "count" : 23, "ok" : 1.0, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1679060278, "i" : 168 } }, "signature" : { "hash" : { "$binary" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "$type" : "00" }, "keyId" : 0 } }, "operationTime" : { "$timestamp" : { "t" : 1679060278, "i" : 168 } } }
  operation_id 16810
  request_id 7
find ... begin
-> command started find
  database_name db
  command { "find" : "coll", "filter" : {  }, "limit" : 1, "$db" : "db", "$readPreference" : { "mode" : "primaryPreferred" }, "lsid" : { "id" : { "$binary" : "5vwfhDpzQGSCzWCoMxjRIA==", "$type" : "04" } }, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1679060278, "i" : 168 } }, "signature" : { "hash" : { "$binary" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "$type" : "00" }, "keyId" : 0 } } }
  operation_id 16811
  request_id 9
<- command failed find
  failure { "ok" : 0.0, "errmsg" : "Failing command via 'failCommand' failpoint", "code" : 11600, "codeName" : "InterruptedAtShutdown", "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1679060278, "i" : 168 } }, "signature" : { "hash" : { "$binary" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "$type" : "00" }, "keyId" : 0 } }, "operationTime" : { "$timestamp" : { "t" : 1679060278, "i" : 168 } } }
  operation_id 16811
  request_id 9
-> command started find
  database_name db
  command { "find" : "coll", "filter" : {  }, "limit" : 1, "$db" : "db", "$readPreference" : { "mode" : "primaryPreferred" }, "lsid" : { "id" : { "$binary" : "5vwfhDpzQGSCzWCoMxjRIA==", "$type" : "04" } }, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1679060278, "i" : 168 } }, "signature" : { "hash" : { "$binary" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "$type" : "00" }, "keyId" : 0 } } }
  operation_id 16811
  request_id 11
<- command succeeded find
  reply { "cursor" : { "firstBatch" : [ { "_id" : { "$oid" : "6414618184a4c96c51084321" }, "foo" : "bar" } ], "id" : 0, "ns" : "db.coll" }, "ok" : 1.0, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1679060279, "i" : 102 } }, "signature" : { "hash" : { "$binary" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "$type" : "00" }, "keyId" : 0 } }, "operationTime" : { "$timestamp" : { "t" : 1679060279, "i" : 102 } } }
  operation_id 16811
  request_id 11
find ... end
-> command started endSessions
  database_name admin
  command { "endSessions" : [ { "id" : { "$binary" : "5vwfhDpzQGSCzWCoMxjRIA==", "$type" : "04" } } ], "$db" : "admin", "$readPreference" : { "mode" : "primaryPreferred" }, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1679060279, "i" : 102 } }, "signature" : { "hash" : { "$binary" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "$type" : "00" }, "keyId" : 0 } } }
  operation_id 16812
  request_id 13
<- command succeeded endSessions
  reply { "ok" : 1.0, "$clusterTime" : { "clusterTime" : { "$timestamp" : { "t" : 1679060279, "i" : 102 } }, "signature" : { "hash" : { "$binary" : "AAAAAAAAAAAAAAAAAAAAAAAAAAA=", "$type" : "00" }, "keyId" : 0 } }, "operationTime" : { "$timestamp" : { "t" : 1679060279, "i" : 102 } } }
  operation_id 16812
  request_id 13
*/