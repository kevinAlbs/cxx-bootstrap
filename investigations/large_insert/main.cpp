/*
Example of attempting to insert a large document.

Sample output:
```
got bulk write exception: object to insert too large. size in bytes: 16777258, max size: 16777216: generic server error
```
*/
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>

#include <cstdlib>
#include <iostream>

using namespace bsoncxx::builder::basic;
int main()
{
    auto instance = mongocxx::instance();
    auto uri = mongocxx::uri("mongodb://localhost:27017");
    auto client = mongocxx::client(uri);
    auto db = client.database("db");
    auto coll = db.collection("coll");
    std::vector<bsoncxx::document::value> docs;
    std::string large_string = "a";
    for (size_t i = 0; i < 16 * 1024 * 1024; i++)
    {
        large_string += "a";
    }
    docs.push_back(make_document(kvp("large_string", large_string)));
    try
    {
        coll.insert_many(docs);
    }
    catch (mongocxx::bulk_write_exception &bwe)
    {
        std::cout << "got bulk write exception: " << bwe.what() << std::endl;
    }
}
