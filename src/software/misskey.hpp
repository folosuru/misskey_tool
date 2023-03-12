//
// Created by folosuru on 2023/03/03.
//
#pragma once
#include <future>
#include "../api.hpp"

#include "nlohmann/json.hpp"


class misskey : public api {

public:
    static const int instance_get_limit ;

    static const int access_thread_limit;

    static const int connect_timeout ;

    using api::api;

    std::optional<instance_list> fetchAllFederation() override;

    int getFederationCount() override;

    api::register_status getRegisterStatus() override;

    static std::optional<instance_list> fetchFederation(const utility::string_t &URL, int offset);


    std::string getDescription() override;

    static const utility::string_t INSTANCES_PATH;
    static const utility::string_t STATS_PASS;
    static const utility::string_t MIME_APPLICATION_JSON;
    static const utility::string_t LIMIT;
    static const utility::string_t OFFSET;
    static const utility::string_t INSTANCES;
    static const utility::string_t LOG_TIMEOUT;
    static const utility::string_t META_PASS;


};