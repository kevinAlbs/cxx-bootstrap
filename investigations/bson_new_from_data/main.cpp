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

    // `bsondata` represents the document { "foo" : "bar" }
    uint8_t bsondata[] = {
        0x12, 0x00, 0x00, 0x00, 0x02, 0x66, 0x6f, 0x6f, 0x00, 0x04, 0x00, 0x00, 0x00, 0x62, 0x61, 0x72, 0x00, 0x00};
    auto doc = bsoncxx::document::view(bsondata, sizeof bsondata);
    // Append as a subdocument:
    auto doc2 = make_document(kvp("subdoc", doc));
    std::cout << bsoncxx::to_json(doc2) << std::endl;
    // Prints `{ "subdoc" : { "foo" : "bar" } }`.
}
