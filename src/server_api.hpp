//
// Created by folosuru on 2023/02/24.
//
#pragma once

#include <string>
#include <optional>

#include <cpprest/http_client.h>
#include <nlohmann/json.hpp>

namespace api {

typedef std::vector<std::string> instance_list;

namespace misskey {

    const int instance_get_limit = 100;

    const int access_thread_limit = 10;

    const int connect_timeout = 5;

    /**
     * get Federation from URL.
     * @param URL
     * @param offset
     * @return std::nullopt mean cannot get data;e.g. server is not misskey...
     */
    std::optional<instance_list> fetchFederation(const utility::string_t &URL, int offset);

    std::optional<instance_list> fetchAllFederation(const utility::string_t &URL);
}

namespace mastodon {
    //TODO...
}

}