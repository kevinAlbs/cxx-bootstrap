#include <mongocxx/client.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>
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
    // Construct a URI string with two dots to get past early validation.
    // Failure to resolve SRV records will result in a null return of `mongoc_client_pool_new`.
    std::string uristr = "mongodb+srv://foo.bar.baz";
    auto pool = mongocxx::pool(mongocxx::uri(uristr));
    auto client = pool.try_acquire();
    if (!client)
    {
        std::cout << "failed to acquire client" << std::endl;
        return 1;
    }
    auto db = (*client)->database("db");
    auto doc = make_document(kvp("ping", 1));
    auto res = db.run_command(doc.view());
    std::cout << "ping replied with " << bsoncxx::to_json(res.view()) << std::endl;
}

/* Sample output:
/Users/kevin.albertson/code/cxx-bootstrap/install/mongo-c-driver-1.23.2-src/mongo-c-driver/src/libmongoc/src/mongoc/mongoc-client-pool.c:346 mongoc_client_pool_try_pop(): precondition failed: pool
./run.sh: line 1: 99814 Abort trap: 6           ./cmake-build/main.out
*/
