#include <mongocxx/client.hpp>
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

    std::string uristr = "mongodb://localhost:27017";
    if (std::getenv("MONGODB_URI"))
    {
        uristr = std::getenv("MONGODB_URI");
    }

    auto client = mongocxx::client(mongocxx::uri(uristr));
    auto db = client.database("db");
    auto coll = db.collection("coll");

    // Iterates the change stream until an error occurs.
    {
        mongocxx::change_stream stream = coll.watch();

        // Track the last resume token.
        bsoncxx::stdx::optional<bsoncxx::document::view> last_resume_token;

        while (true)
        {
            bool invalidated = false;

            // Iterate next batch of events.
            for (const auto &event : stream)
            {
                std::cout << bsoncxx::to_json(event) << std::endl;
                last_resume_token = stream.get_resume_token();

                if (event["operationType"].get_string().value == "invalidate")
                {
                    invalidated = true;
                    break;
                }
            }

            if (invalidated)
            {
                std::cout << "got 'invalidate', recreating stream" << std::endl;
                assert(last_resume_token.has_value());
                mongocxx::options::change_stream options;
                options.start_after(*last_resume_token);
                stream = coll.watch(options);
                continue;
            }

            last_resume_token = stream.get_resume_token();
            std::cout << "Empty batch of events. Trying again..." << std::endl;
        }
    }
}
