//
// Created by folosuru on 2023/03/06.
//

#pragma once

#include <string>
#include <unordered_set>
#include "sql.hpp"
#include "util.hpp"

namespace util {
    namespace blacklist {


        /*
         * black list domain
         */
        const std::unordered_set<std::string> blacklisted_domain = {
                "activitypub-troll.cf",
                "misskey-forkbomb.cf",
                "repl.co",
                "gab.best",
                "ngrok.io",
                ".local"
        };

        const std::unordered_set<std::string> blacklisted_ip = {
                "10.",
                "192.168.",
        };

        bool isBlacklisted(pqxx::connection &connection,const std::string& url);

    }
}