#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <cstdlib>
#include <iostream>

using namespace bsoncxx::builder::basic;
int main()
{
    auto instance = mongocxx::instance();

    std::string uristr = "mongodb://localhost:27017";
    if (std::getenv("MONGODB_URI"))
    {
        uristr = std::getenv("MONGODB_URI");
    }

    auto client_opts = mongocxx::options::client();
    auto apm_opts = mongocxx::options::apm();

    apm_opts.on_command_succeeded(
        [](const mongocxx::events::command_succeeded_event &event)
        {
            auto timestamp = event.reply()["operationTime"].get_timestamp();
            std::cout << "command " << event.command_name()
                      << " succeeded with operationTime (" << timestamp.timestamp
                      << "," << timestamp.increment << ")" << std::endl;
        });

    client_opts.apm_opts(apm_opts);

    auto client = mongocxx::client(mongocxx::uri(uristr), client_opts);
    auto db = client.database("db");
    auto coll = db.collection("coll");

    // Iterates the change stream until an error occurs.
    std::cout << "Watching collection db.coll for notifications ..." << std::endl;
    {
        mongocxx::change_stream stream = coll.watch();

        // Track the last resume token.
        bsoncxx::stdx::optional<bsoncxx::document::view> last_resume_token;

        while (true)
        {
            bool invalidated = false;

            bool has_notifications = false;
            // Iterate next batch of notifications (possibly empty).
            for (const auto &notification : stream)
            {
                std::cout << bsoncxx::to_json(notification) << std::endl;
                last_resume_token = stream.get_resume_token();

                if (notification["operationType"].get_string().value == "invalidate")
                {
                    invalidated = true;
                    break;
                }
            }

            if (!has_notifications)
            {
                std::cout << "Empty batch of notifications. Trying again..."
                          << std::endl;
            }

            last_resume_token = stream.get_resume_token();
            assert(last_resume_token);
            std::cout << "Last resume token: " << bsoncxx::to_json(*last_resume_token)
                      << std::endl;
        }
    }
}
