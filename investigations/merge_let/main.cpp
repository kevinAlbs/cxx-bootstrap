// Example of using $merge with `let` and a `whenMatched` pipeline.

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
    auto coll_from = db["from"];
    auto coll_to = db["to"];
    // Drop prior data
    {
        coll_from.drop();
        coll_to.drop();
    }

    // Insert documents into both collections with matching values.
    {
        coll_from.insert_one(make_document(kvp("_id", "matches")));
        coll_to.insert_one(make_document(kvp("_id", "matches")));
    }
    // Insert a document into `db.from` with a different value.
    coll_from.insert_one(make_document(kvp("_id", "does not match")));

    auto pipeline = mongocxx::pipeline();
    pipeline.merge(
        make_document(
            kvp("into", "to"),
            kvp("on", "_id"),
            // On a match, run the pipeline (with let variables)
            kvp("let", make_document(kvp("foo", "bar"))),
            kvp("whenMatched", make_array(
                                   make_document(
                                       kvp("$addFields",
                                           // Reference `foo` variable defined with `let`.
                                           make_document(kvp("newField", "$$foo")))))),
            // Otherwise, insert the given doc.
            kvp("whenNotMatched", "insert")));
    coll_from.aggregate(pipeline).begin(); // call `begin` to iterate the cursor and send the pipeline.

    std::cout << "After $merge, `db.to` contains:" << std::endl;
    for (auto &&doc : coll_to.find(make_document()))
    {
        std::cout << bsoncxx::to_json(doc, bsoncxx::ExtendedJsonMode::k_relaxed) << std::endl;
    }
}

// Sample output:
// After $merge, `db.to` contains:
// { "_id" : "matches", "newField" : "bar" }
// { "_id" : "does not match" }
