//
// Created by folosuru on 2023/03/06.
//

#pragma once

#include <string>
#include <unordered_set>
#include <cstdint>
#include <shared_mutex>
#include "sql.hpp"
#include "util.hpp"

namespace util {
class blacklist {
public:
    explicit blacklist(pqxx::connection& connection);

    blacklist() = default;

    [[nodiscard]]
    bool isBlacklisted(const std::string& url) const noexcept;

    bool addBlacklistCandidate(const std::string& url);
private:
    /*
     * black list domain
     */
    static inline const std::unordered_set<std::string> domain = {
            "activitypub-troll.cf",
            "misskey-forkbomb.cf",
            "repl.co",
            "ngrok.io",
            ".local",
            "eeeeeeeee.eu",
            ".bka.li",
            ".kromonos.net"
    };

    static inline const std::unordered_set<std::string> ip = {
            "10.",
            "192.168.",
    };

    std::unordered_set<std::string> blacklisted;
    std::unordered_map<std::string, std::int_fast8_t> blacklist_candidate_score;
    static constexpr std::int_fast8_t blacklist_threshold = 10;
    mutable std::shared_mutex mutex;
};
}
