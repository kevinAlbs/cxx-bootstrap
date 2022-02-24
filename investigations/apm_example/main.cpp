#include <mongocxx/client.hpp>
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

    auto client_opts = mongocxx::options::client();
    auto apm_opts = mongocxx::options::apm();

    apm_opts.on_command_started ([](const mongocxx::events::command_started_event& event) {
        std::cout << "-> command started " << event.command_name() << " " << event.database_name() << " " << bsoncxx::to_json (event.command()) << std::endl;
    });

    apm_opts.on_command_succeeded ([](const mongocxx::events::command_succeeded_event& event) {
        std::cout << "<- command succeeded " << event.command_name() << " " << bsoncxx::to_json (event.reply()) << std::endl;
    });

    apm_opts.on_command_failed ([](const mongocxx::events::command_failed_event& event) {
        std::cout << "<- command failed " << event.command_name() << " " << bsoncxx::to_json (event.failure()) << std::endl;
    });

    client_opts.apm_opts(apm_opts);
    
    auto client = mongocxx::client(mongocxx::uri(uristr), client_opts);
    auto db = client.database("db");
    auto doc = make_document(kvp("ping", 1));
    auto res = db.run_command (doc.view());
    std::cout << "ping replied with " << bsoncxx::to_json (res.view()) << std::endl;
}
