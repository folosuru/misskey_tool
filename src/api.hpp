//
// Created by folosuru on 2023/02/24.
//
#pragma once

#include <optional>

#include <cpprest/http_client.h>
#include <nlohmann/json.hpp>

class api {

public:

    static api * getInstance(const std::string& URL);

    typedef std::vector<std::string> instance_list;

    api() = delete;
    api(const utility::string_t& URL , nlohmann::json nodeinfo , nlohmann::json manifest);

    virtual std::optional<instance_list> fetchAllFederation();

    virtual int getFederationCount();

    virtual std::string getDescription();

    utility::string_t getURL();

    std::string getServerSoftware();

    std::string getSummary();

    int getUserCount();

    int getPostsCount();

    // string for cpprestsdk...
    static const utility::string_t HTTP_URI_SCHEME; // "https://"
    static const utility::string_t NODEINFO_PATH;   // "/.well-known/nodeinfo"
    static const utility::string_t MANIFEST_PATH;   // "/manifest.json"
    static const utility::string_t LINKS;   // "links"
    static const utility::string_t HREF;   // "href"


protected:

    utility::string_t URL;

    nlohmann::json nodeinfo;

    nlohmann::json manifest;

    std::optional<int> FederationCount = std::nullopt;

};