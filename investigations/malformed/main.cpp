// Test sending a command after the `mongocxx::instance` is destroyed.
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>

#include <iostream>

using namespace bsoncxx::builder::basic;
int main()
{
    {
        // Create and destroy an instance.
        auto instance = mongocxx::instance();
    }

    auto client = mongocxx::client(mongocxx::uri("mongodb://localhost:27017"));
    auto db = client.database("db");
    auto doc = make_document(kvp("ping", 1));
    auto res = db.run_command(doc.view());
    std::cout << "ping replied with " << bsoncxx::to_json(res.view()) << std::endl;
}

/*
Destroying the `mongocxx::instance` frees global handshake data.
Inspecting the logs of mongod shows a malformed handshake (see `platform` field):

{
    "t": {
        "$date": "2024-02-09T11:32:49.555-05:00"
    },
    "s": "I",
    "c": "NETWORK",
    "id": 51800,
    "ctx": "conn499",
    "msg": "client metadata",
    "attr": {
        "remote": "127.0.0.1:52825",
        "client": "conn499",
        "negotiatedCompressors": [],
        "doc": {
            "driver": {
                "name": "",
                "version": ""
            },
            "os": {
                "type": "Darwin",
                "name": "macOS",
                "version": "",
                "architecture": "file"
            },
            "platform": "\u0001mongodb://localhost:27017"
        }
    }
}
*/
