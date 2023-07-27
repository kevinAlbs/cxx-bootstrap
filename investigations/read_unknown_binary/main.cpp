#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <cstdlib>
#include <iostream>

using bsoncxx::binary_sub_type;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::types::b_binary;

int main()
{
    auto instance = mongocxx::instance();

    // Parse JSON with an unknown BSON binary subtype 0x09
    {
        auto got = bsoncxx::from_json(R"({"from_json": { "$binary": { "base64": "AAAA", "subType": "09" }}})");
        // Print it back.
        std::cout << bsoncxx::to_json(got.view()) << std::endl;
        // Prints: { "from_json" : { "$binary" : "AAAA", "$type" : "09" } }
    }

    // Make a BSON document with an unknown BSON binary subtype 0x09
    {
        const uint8_t bytes[] = {0x01, 0x02, 0x03, 0x04};
        auto doc = make_document(kvp("from_make_document", b_binary{.sub_type = (binary_sub_type)0x09, .bytes = bytes, .size = 4}));
        std::cout << bsoncxx::to_json(doc.view()) << std::endl;
        // Prints: { "from_make_document" : { "$binary" : "AQIDBA==", "$type" : "09" } }
    }
}
