//
// Created by folosuru on 2023/02/24.
//
#pragma once

#include <string>
#include <optional>

#include <cpprest/http_client.h>
#include <nlohmann/json.hpp>



namespace misskey{
    const int instance_get_limit = 100;

    /**
     * get Federation from URL.
     * @param URL
     * @param offset
     * @return
     */
    std::optional<std::vector<std::string>> fetchFederation(const utility::string_t&  URL,int offset);

};
