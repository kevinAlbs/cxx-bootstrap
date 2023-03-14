#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>

#include <cstdlib>
#include <iostream>

using namespace bsoncxx::builder::basic;
int main()
{
    auto instance = mongocxx::instance();

    std::string uristr = "mongodb://localhost:27017";
    if (std::getenv("MONGODB_URI"))
    {
        uristr = std::getenv("MONGODB_URI");
    }

    auto client = mongocxx::client(mongocxx::uri(uristr));
    auto db = client.database("db");
    auto doc = make_document(kvp("ping", 1));
    auto res = db.run_command(doc.view());
    std::cout << "ping replied with " << bsoncxx::to_json(res.view()) << std::endl;
}

/*
Here is an example formatted output from mongod logs for the C++ driver 3.7.0:

{
    "t": {
        "$date": "2023-03-14T09:01:16.710-04:00"
    },
    "s": "I",
    "c": "NETWORK",
    "id": 51800,
    "ctx": "conn1014",
    "msg": "client metadata",
    "attr": {
        "remote": "127.0.0.1:62641",
        "client": "conn1014",
        "doc": {
            "driver": {
                "name": "mongoc / mongocxx",
                "version": "1.23.2 / 3.7.0"
            },
            "os": {
                "type": "Darwin",
                "name": "macOS",
                "version": "21.6.0",
                "architecture": "arm64"
            },
            "platform": "cfg=0x0300d6aa65 posix=200112 stdc=201710 CC=clang 13.1.6 (clang-1316.0.21.2.5) CFLAGS=\"\" LDFLAGS=\"\""
        }
    }
}

*/