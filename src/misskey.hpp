//
// Created by folosuru on 2023/02/24.
//
#pragma once

#include <string>
#include <optional>
#include <cpprest/http_client.h>
#include <optional>


class misskey {

public:
    static std::string fetchFederation(const utility::string_t&  URL);
};
