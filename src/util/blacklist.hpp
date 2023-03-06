//
// Created by folosuru on 2023/03/06.
//

#pragma once

#include <string>
#include <vector>

namespace util {
    namespace blacklist {


        /*
         * black list domain
         */
        const std::vector<std::string> blacklisted = {
                "activitypub-troll.cf",
                "misskey-forkbomb.cf",
                "repl.co"
        };

        bool isBlacklisted(std::string url);

    };
}