//
// Created by folosuru on 2023/03/06.
//

#include "blacklist.hpp"

bool util::blacklist::isBlacklisted(pqxx::connection &connection, const std::string& url) {
    for (const auto &i: blacklisted_domain) {
        if (url.length() > i.length()) {
            if (url.substr(url.length() - i.length()) == i) {
                return true;
            }
        }
    }
    for (const auto &i: blacklisted_ip) {
        if (url.length() > i.size()) {
            if (url.substr(0, i.size()) == i) {
                return true;
            }
        }
    }
    return util::sql::isExistInBlacklist(connection, util::getTopLevelDomain(url));
}
