#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/exception/query_exception.hpp>

#include <cstdlib>
#include <iostream>

using namespace bsoncxx::builder::basic;
int main()
{
    mongocxx::instance instance;
    // mongocxx::uri connectionString = mongocxx::uri("mongodb+srv://bootstrapmongodb.4lwdw.mongodb.net/myFirstDatabase?replicaSet=atlas-knh6k8-shard-0&authMechanism=MONGODB-X509&ssl=true&retryWrites=true&tlsCertificateKeyFile=C:\\cygwin\\home\\Administrator\\secrets\\X509-cert-5915913444876671085.pem");
    // Try a non SRV connection string.
    // mongocxx::uri connectionString = mongocxx::uri("mongodb://bootstrapmongodb-shard-00-00.4lwdw.mongodb.net/?authMechanism=MONGODB-X509&ssl=true&retryWrites=true&tlsCertificateKeyFile=C:\\cygwin\\home\\Administrator\\secrets\\X509-cert-5915913444876671085.pem");
    // std::cout << "Try a non SRV connection string with a Unix path." << std::endl;
    // mongocxx::uri connectionString = mongocxx::uri("mongodb://bootstrapmongodb-shard-00-00.4lwdw.mongodb.net/?authMechanism=MONGODB-X509&ssl=true&retryWrites=true&tlsCertificateKeyFile=/home/Administrator/secrets/X509-cert-5915913444876671085.pem");
    std::cout << "Try a non SRV connection string with a relative path with pkcs1." << std::endl;
    mongocxx::uri connectionString = mongocxx::uri("mongodb://bootstrapmongodb-shard-00-00.4lwdw.mongodb.net/?authMechanism=MONGODB-X509&ssl=true&retryWrites=true&tlsCertificateKeyFile=client.pem");
   
    mongocxx::options::tls tls_opts{};
    // tls_opts.pem_file("C:\\cygwin\\home\\Administrator\\secrets\\X509-cert-5915913444876671085.pem");
    // tls_opts.pem_file("/home/Administrator/secrets/X509-cert-5915913444876671085.pem");
    mongocxx::options::client client_opts{};
    // client_opts.tls_opts(tls_opts);

    auto client = mongocxx::client{connectionString, client_opts};
    mongocxx::database db = client["testDB"];
    mongocxx::collection collection = db["testCol"];
    try {
        std::cout << collection.count_documents({});
    } catch (mongocxx::query_exception& qe) {
        auto raw_server_error = qe.raw_server_error();
        if (raw_server_error) {
            std::cout << "raw_server_error=" << bsoncxx::to_json (*raw_server_error) << std::endl;
        }
        std::cout << qe.what() << std::endl;
    } catch (std::exception& e) {
        std::cout << "unexpected exception" << std::endl;
    }
}
