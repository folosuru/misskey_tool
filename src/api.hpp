//
// Created by folosuru on 2023/02/24.
//
#pragma once

#include <string>
#include <optional>

#include <cpprest/http_client.h>

class api {

public:

    static api * getInstance(const utility::string_t& URL);

    typedef std::vector<std::string> instance_list;

    api() = delete;
    api(const utility::string_t& URL , web::json::value nodeinfo);

    virtual std::optional<instance_list> fetchAllFederation() = 0;

    virtual int getFederationCount() = 0;

    utility::string_t getURL();

private:

    utility::string_t URL;

    web::json::value nodeinfo;

};