//
// Created by folosuru on 2023/03/06.
//

#include "blacklist.hpp"
util::blacklist::blacklist(pqxx::connection &connection) {
    pqxx::work work(connection);
    for (auto [domain] : tx.query<std::string, int>(
        "select text from blacklist where domain;")) {
        blacklisted.insert(domain);
    }
}

bool util::blacklist::isBlacklisted(const std::string& url) {
    for (const auto &i: blacklist::domain) {
        if (url.length() > i.length()) {
            if (url.substr(url.length() - i.length()) == i) {
                return true;
            }
        }
    }
    for (const auto &i: blacklist::ip) {
        if (url.length() > i.size()) {
            if (url.substr(0, i.size()) == i) {
                return true;
            }
        }
    }
    return (blacklisted.find(url) != blacklisted.end());
}
