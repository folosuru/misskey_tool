//
// Created by folosuru on 2023/02/24.
//
#pragma once

#include <optional>

#include <cpprest/http_client.h>
#include <nlohmann/json.hpp>

class api {

public:

    static api * getInstance(const utility::string_t& URL);

    typedef std::vector<std::string> instance_list;

    api() = delete;
    api(const utility::string_t& URL , nlohmann::json nodeinfo);

    virtual std::optional<instance_list> fetchAllFederation() = 0;

    virtual int getFederationCount() = 0;

    utility::string_t getURL();

private:

    utility::string_t URL;

    nlohmann::json nodeinfo;

protected:

    std::optional<int> FederationCount = std::nullopt;

};