#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/change_stream.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/options/change_stream.hpp>

#include <cstdlib>
#include <iostream>

using namespace bsoncxx::builder::basic;
using namespace mongocxx;
int main () {
    auto instance = mongocxx::instance();

    std::string uristr = "mongodb://localhost:27017";
    if (std::getenv ("MONGODB_URI")) {
        uristr = std::getenv ("MONGODB_URI");
    }

    mongocxx::options::change_stream options;
    mongocxx::pipeline pipeline;
    mongocxx::client client = mongocxx::client(mongocxx::uri(uristr));
    auto db = client["db"];
    options.start_at_operation_time(bsoncxx::types::b_timestamp{1, 1643812827});
    // Use a small batch_size to simulate a segfault.
    // [ 1, 2, 3 ] [ 4, 5, 6]
    //        a^
    //              b^
    // A bsoncxx::document::view on `a` is invalid after incrementing to a document
    // on the next batch. If the next batch is empty, the C++ driver sets it to
    // an empty document.
    options.batch_size(1);

    int count = 0;     
    change_stream stream = db.watch(pipeline, options);     
    auto cs_iter = stream.begin();     
    bsoncxx::document::view event;     
    while (true) {         
    if (cs_iter == stream.end()) {             
        std::cout << "No event yet." << std::endl;             
        cs_iter = stream.begin(); // sends at most one getMore command.  
        continue;
    } else {             
        std::cout << count << std::endl;             
        event = *cs_iter;                 
        cs_iter++; // sends at most one getMore command.         
    }
    auto eventLen = event.length(); 
    std::cout << "eventLen " << eventLen << std::endl; 
    // if (count == 11197) {             
        std::cout << bsoncxx::to_json(event) << std::endl;         
    // }         
    auto f2 = event.find("ns");         
    std::cout << "f2" << std::endl;         
    count++;         
    // do more stuff with event...      
    }    
}
