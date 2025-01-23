#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>

std::string hexEncode(uint8_t *data, size_t len) {
  std::stringstream ss;
  for (size_t i = 0; i < len; i++) {
    printf("data[i] = %d\n", data[i]);
    ss << std::setw(2) << std::setfill('0') << std::hex << (uint32_t)data[i];
  }
  return ss.str();
}

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::sub_array;

// Stores an array of bytes
void write_byte_array() {
  uint8_t data[] = {'\x00', '\x01', '\x02'};
  auto document = make_document(kvp("data", [&data](sub_array child) {
    for (const auto &element : data)
      child.append(element); // Values are stored as BSON int32.
  }));
  std::cout << bsoncxx::to_json(document.view()) << std::endl;
  // Prints: { "data" : [ 0, 1, 2 ] }
}

// Stores a BSON binary.
void write_bson_binary() {
  uint8_t data[] = {'\x00', '\x01', '\x02'};
  bsoncxx::types::b_binary data_binary{bsoncxx::binary_sub_type::k_binary, 3, data};
  auto document = make_document(kvp("data", data_binary));
  std::cout << bsoncxx::to_json(document.view()) << std::endl;
  // Prints: { "data" : { "$binary" : "AAEC", "$type" : "00" } }
}

// Hex encode binary data, store it, then search for it.
void write_bson_hex() {
  std::string uristr = "mongodb://localhost:27017";
  auto client = mongocxx::client(mongocxx::uri(uristr));
  auto coll = client.database("db").collection("coll");
  coll.drop();

  uint8_t data[] = {'\x00', '\x01', '\x02'};
  size_t data_len = sizeof(data);
  std::string hexEncoded = hexEncode(data, data_len); // Encodes to the string "000102"
  coll.insert_one(make_document(kvp("data", hexEncoded)));
  // Search for substring of hex "0102".
  auto filter = make_document(kvp("data", bsoncxx::types::b_regex("0102")));
  auto got = coll.find_one(filter.view());
  assert(got);
  std::cout << "found " << bsoncxx::to_json(got->view()) << std::endl;
}

using namespace bsoncxx::builder::basic;
int main() {
  auto instance = mongocxx::instance();

  write_byte_array();
  write_bson_binary();
  write_bson_hex();
}
