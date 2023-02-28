//
// Created by folosuru on 2023/02/27.
//

#include "../api.hpp"

class other : public api {

    using api::api;

    std::optional<instance_list> fetchAllFederation() override {
        return std::nullopt;
    }

    int getFederationCount() override {
        return 0;
    }



};