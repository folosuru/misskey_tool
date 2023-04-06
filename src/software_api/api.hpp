//
// Created by folosuru on 2023/02/24.
//
#pragma once

#include <optional>
#include <unordered_set>

#include <cpprest/http_client.h>
#include <nlohmann/json.hpp>

class api {

public:

    virtual ~api() = default;

    static api * getInstance(const std::string& URL);

    typedef std::unordered_set<std::string> instance_list;

    enum register_status {
        everyone,
        invite,
        approval,
        deny,
        unknown,
    };

    api() = delete;
    api(const utility::string_t& URL , nlohmann::json nodeinfo , nlohmann::json manifest);

    virtual std::optional<instance_list> fetchAllFederation();

    virtual int getFederationCount();

    virtual std::string getDescription();

    utility::string_t getURL();

    std::string getDomain();

    virtual std::string getName();

    virtual api::register_status getRegisterStatus();

    std::string getServerSoftware();


    virtual int getUserCount();

    virtual int getPostsCount();

    virtual std::string getIcon();

    virtual std::string getServerVersion();

    virtual std::string getBanner();

    // string for cpprestsdk...
    static const utility::string_t HTTPS_URI_SCHEME; // "https://"
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