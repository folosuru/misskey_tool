//
// Created by folosuru on 2023/03/06.
//

#include "blacklist.hpp"

bool util::blacklist::isBlacklisted(std::string url) {
    for (auto i : blacklisted){
        if (url.substr(url.length() - i.length()) == i){
            return true;
        }
    }
    return false;
}
