#pragma once

#include <mongocxx/options/apm.hpp>

static inline mongocxx::options::apm get_apm_opts(bool started_only = true)
{
    mongocxx::options::apm opts;

    opts.on_command_started([](const mongocxx::events::command_started_event &event)
                            { std::cout << "-> command started " << event.command_name() << " " << event.database_name() << " " << bsoncxx::to_json(event.command()) << std::endl; });

    if (started_only)
        return opts;

    opts.on_command_succeeded([](const mongocxx::events::command_succeeded_event &event)
                              { std::cout << "<- command succeeded " << event.command_name() << " " << bsoncxx::to_json(event.reply()) << std::endl; });

    opts.on_command_failed([](const mongocxx::events::command_failed_event &event)
                           { std::cout << "<- command failed " << event.command_name() << " " << bsoncxx::to_json(event.failure()) << std::endl; });

    opts.on_server_changed([](const mongocxx::events::server_changed_event &event)
                           {
        std::cout << "server changed " << event.host() << ":" << event.port() << " changed from " << event.previous_description().type() << " to " << event.new_description().type() << std::endl;
        std::cout << "most recent ismaster " << bsoncxx::to_json(event.new_description().hello()) << std::endl; });

    return opts;
}