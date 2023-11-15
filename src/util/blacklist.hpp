//
// Created by folosuru on 2023/03/06.
//

#pragma once

#include <string>
#include <unordered_set>
#include "sql.hpp"
#include "util.hpp"

namespace util {
class blacklist {
public:
    blacklist(pqxx::connection &connection);

    bool isBlacklisted(const std::string& url);
private:
    /*
     * black list domain
     */
    static inline const std::unordered_set<std::string> domain = {
            "activitypub-troll.cf",
            "misskey-forkbomb.cf",
            "repl.co",
            "gab.best",
            "ngrok.io",
            ".local"
    };

    static inline const std::unordered_set<std::string> ip = {
            "10.",
            "192.168.",
    };

    std::unordered_set<std::string> blacklisted;
    }
}
