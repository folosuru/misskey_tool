//
// Created by folosuru on 2023/03/06.
//
#pragma once
#include <future>
#include "../api.hpp"

#include "nlohmann/json.hpp"

class mastodon : public api {
public:

    using api::api;

    std::string getDescription() override;

    std::optional<instance_list> fetchAllFederation() override;

    int getFederationCount() override;

    std::vector<std::string> getPeers();

private:
    static const utility::string_t API_V1_INSTANCE;
    static const utility::string_t API_V2_INSTANCE;
    static const utility::string_t API_PEERS;
    std::optional<instance_list> peers;
};
