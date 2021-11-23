#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/change_stream.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>

#include <cstdlib>
#include <iostream>

using namespace bsoncxx::builder::basic;
int main () {
    auto instance = mongocxx::instance();

    std::string uristr = "mongodb://localhost:27017";
    if (std::getenv ("MONGODB_URI")) {
        uristr = std::getenv ("MONGODB_URI");
    }

    auto client = mongocxx::client(mongocxx::uri(uristr));
    auto db = client.database("db");
    auto cs = db.collection("coll").watch();
    auto cs_iter = cs.begin(); // sends at most one aggregate command.
    while (true) {
        if (cs_iter == cs.end()) {
            std::cout << "No event yet." << std::endl;
            cs_iter = cs.begin (); // sends at most one getMore command.
        } else {
            bsoncxx::document::view event = *cs_iter;
            std::cout << "Got event: " << bsoncxx::to_json (event) << std::endl;
            cs_iter++; // sends at most one getMore command.
        }
    }
    // Note: if a resumable error occurs while iterating a change stream, there
    // may be repeated attempts of the aggregate command in any of the begin or
    // iteration calls.
}
