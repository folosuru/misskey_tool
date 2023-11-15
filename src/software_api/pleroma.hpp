//
// Created by folosuru on 2023/03/12.
//
#pragma once
#include "api.hpp"

class pleroma : public api {
public:

    ~pleroma() = default;

    enum class api_version {
        v1,
        v2
    };

    using api::api;

    std::string getDescription() override;

    std::optional<instance_list> fetchAllFederation() override;

    void fetchFederationToQueue() override;

    int getFederationCount() override;

    std::optional<api::instance_list> getPeers();

    std::string getIcon() override;

    std::string getName() override;

    std::string getBanner() override;

    std::string getServerVersion() override;

    std::pair<nlohmann::json, pleroma::api_version> getInstance();

private:
    static const utility::string_t API_V1_INSTANCE;
    static const utility::string_t API_V2_INSTANCE;
    static const utility::string_t API_PEERS;
    std::optional<std::optional<instance_list>> peers;
    std::optional<std::pair<nlohmann::json , api_version>> instance;
};
