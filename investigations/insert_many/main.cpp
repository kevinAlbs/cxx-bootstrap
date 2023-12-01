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
    // `run_command` inserts without using an OP_MSG document sequence:
    db.run_command(
        make_document(
            kvp("insert", "coll"),
            kvp("documents", make_array(
                                 make_document(kvp("a", 1)),
                                 make_document(kvp("a", 2))))));
    // `insert_many` inserts with an OP_MSG document sequence:
    auto docs = std::vector<bsoncxx::document::value>{
        make_document(kvp("a", 1)),
        make_document(kvp("a", 2)),
    };
    db.collection("coll").insert_many(docs);
}
