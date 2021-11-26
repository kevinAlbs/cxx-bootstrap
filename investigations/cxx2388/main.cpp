#include <mongocxx/client.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/document.hpp>
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

    auto tls_opts = mongocxx::options::tls{};
    tls_opts.ca_file("../../x509gen/ca.pem");
    tls_opts.pem_file("../../x509gen/client.pem");

    {
        std::cout << "Attempting to configure with a mongocxx::client" << std::endl;
        auto client_opts = mongocxx::options::client{};
        client_opts.tls_opts (tls_opts);

        auto client = mongocxx::client(mongocxx::uri(uristr), client_opts);
        auto db = client.database("db");
        auto doc = make_document(kvp("ping", 1));
        auto res = db.run_command (doc.view());
        std::cout << "ping replied with " << bsoncxx::to_json (res.view()) << std::endl;
    }

    {
        std::cout << "Attempting to configure with a mongocxx::pool" << std::endl;
        auto client_opts = mongocxx::options::client{};
        client_opts.tls_opts (tls_opts);
        auto pool_opts = mongocxx::options::pool{client_opts};

        auto pool = mongocxx::pool(mongocxx::uri(uristr), pool_opts);
        auto client = pool.acquire();
        auto db = client->database("db");
        auto doc = make_document(kvp("ping", 1));
        auto res = db.run_command (doc.view());
        std::cout << "ping replied with " << bsoncxx::to_json (res.view()) << std::endl;
    }
}

/* Sample output:

$ MONGODB_URI='mongodb://username:password@localhost:27017/admin?appName=Genny&maxPoolSize=2000&socketTimeoutMS=-1&tls=true&tlsAllowInvalidHostnames=true' ./run.sh
Attempting to configure with a mongocxx::client
ping replied with { "ok" : 1.0 }
Attempting to configure with a mongocxx::pool
ping replied with { "ok" : 1.0 }

$ MONGODB_URI='mongodb://username:password@localhost:27017/admin?appName=Genny&maxPoolSize=2000&socketTimeoutMS=-1&ssl=true&tlsAllowInvalidHostnames=true' ./run.sh
Attempting to configure with a mongocxx::client
ping replied with { "ok" : 1.0 }
Attempting to configure with a mongocxx::pool
ping replied with { "ok" : 1.0 }

*/