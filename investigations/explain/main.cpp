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
    // make document: { "explain": { "find": "coll", "filter": {} } }
    auto doc = make_document(kvp("explain", make_document(kvp("find", "coll"), kvp("filter", make_document()))));
    auto res = db.run_command(doc.view());
    std::cout << "reply: " << bsoncxx::to_json(res.view()) << std::endl;
}
