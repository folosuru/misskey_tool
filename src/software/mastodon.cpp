//
// Created by folosuru on 2023/03/06.
//

#include "mastodon.hpp"

std::string mastodon::getDescription() {
    std::string description_v1 = web::http::client::http_client(getURL() + API_V1_INSTANCE)
        .request(web::http::methods::GET).get().extract_utf8string().get();
    if (description_v1[0] == '[' || description_v1[0] == '{') {
        return nlohmann::json::parse(description_v1)["short_description"].get<std::string>();
    }
    std::string description_v2 = web::http::client::http_client(getURL() + API_V2_INSTANCE)
            .request(web::http::methods::GET).get().extract_utf8string().get();
    if (description_v2[0] == '[' || description_v2[0] == '{') {
        return nlohmann::json::parse(description_v2)["description"].get<std::string>();
    }
    return "";
}

const utility::string_t mastodon::API_V1_INSTANCE = utility::conversions::to_string_t("/api/v1/instance");
const utility::string_t mastodon::API_V2_INSTANCE = utility::conversions::to_string_t("/api/v2/instance");
const utility::string_t mastodon::API_PEERS = utility::conversions::to_string_t("/api/v1/instance/peers");


std::optional<api::instance_list> mastodon::fetchAllFederation() {
    return getPeers();
}

api::instance_list mastodon::getPeers() {
    if (peers){
        return peers.value();
    }

    nlohmann::json json = nlohmann::json::parse(
            web::http::client::http_client(getURL() + API_PEERS)
            .request(web::http::methods::GET).get().extract_utf8string().get()
            );
    peers = instance_list();
    for (auto& i : json){
        peers->push_back(i.get<std::string>());
    }
    return peers.value();
}

int mastodon::getFederationCount() {
    return static_cast<int>(getPeers().size());
}
