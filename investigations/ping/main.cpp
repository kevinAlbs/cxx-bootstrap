#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "../../util/util.hpp"

#include <cstdlib>
#include <iostream>

using namespace bsoncxx::builder::basic;
int main()
{

    auto instance = mongocxx::instance(bsoncxx::stdx::make_unique<stream_logger>(&std::cout));

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
