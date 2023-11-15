//
// Created by folosuru on 2023/02/24.
//
#pragma once

#include <optional>
#include <vector>

#include <cpprest/http_client.h>
#include <nlohmann/json.hpp>
#include "../queue/work_queue.hpp"
#include <memory>
#include "../util/blacklist.hpp"
namespace util {
class blacklist;
}
class api {

public:

    virtual ~api() = default;

    static std::shared_ptr<api> getInstance(const target_domain&,
                                            const std::shared_ptr<util::blacklist>& blacklist);

    typedef std::vector<std::string> instance_list;

    enum register_status {
        everyone,
        invite,
        approval,
        deny,
        unknown,
    };

    api() = delete;
    api(target_domain domain,
        std::shared_ptr<util::blacklist>,
        nlohmann::json nodeinfo,
        nlohmann::json manifest);

    virtual std::optional<instance_list> fetchAllFederation();

    virtual void fetchFederationToQueue();

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
    target_domain domain;

    utility::string_t URL;

    nlohmann::json nodeinfo;

    nlohmann::json manifest;
    std::optional<int> FederationCount = std::nullopt;

    std::shared_ptr<util::blacklist> blacklist_;
};
