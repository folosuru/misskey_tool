#include "blacklist.hpp"

#include <pqxx/pqxx>
util::blacklist::blacklist(pqxx::connection &connection) {
    pqxx::work work(connection);
    for (auto [new_domain] : work.query<std::string>(
        "select domain from blacklist;")) {
        blacklisted.insert(new_domain);
    }
}

bool util::blacklist::isBlacklisted(const std::string& url) const noexcept {
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
    return false;
    std::shared_lock<std::shared_mutex> lock(mutex);
    return (blacklisted.find(url) != blacklisted.end());
}

bool util::blacklist::addBlacklistCandidate(const std::string& url) {
    if (auto it = blacklist_candidate_score.find(url);
        it != blacklist_candidate_score.end()) {
        it->second += 1;
        if (it->second > blacklist_threshold) {
            blacklist_candidate_score.erase(url);
            blacklisted.insert(url);
            return true;
        }
    }
    blacklist_candidate_score.insert({url, 1});
    return false;
}
