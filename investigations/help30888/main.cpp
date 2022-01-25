#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>

#include <cstdlib>
#include <iostream>

using namespace bsoncxx::builder::basic;
int main()
{
    mongocxx::instance instance;
    mongocxx::uri connectionString = mongocxx::uri("mongodb+srv://bootstrapmongodb.4lwdw.mongodb.net/myFirstDatabase?replicaSet=atlas-knh6k8-shard-0&authMechanism=MONGODB-X509&ssl=true&retryWrites=true");

    mongocxx::options::tls tls_opts{};
    tls_opts.pem_file("/Users/kevin.albertson/.secrets/help30888/X509-cert-5915913444876671085.pem");
    mongocxx::options::client client_opts{};
    client_opts.tls_opts(tls_opts);

    auto client = mongocxx::client{connectionString, client_opts};
    mongocxx::database db = client["testDB"];
    mongocxx::collection collection = db["testCol"];
    std::cout << collection.count_documents({});
}
