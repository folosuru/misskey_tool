//
// Created by folosuru on 2023/03/06.
//

#include "mastodon.hpp"

std::string mastodon::getDescription() {

    if (getInstance().second == mastodon::api_version::v2){
        return getInstance().first["description"].get<std::string>();
    }

    if (getInstance().second == mastodon::api_version::v1){
        return getInstance().first["short_description"].get<std::string>();
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

std::pair<nlohmann::json, mastodon::api_version> mastodon::getInstance() {
    if (instance) {
        return instance.value();
    }

    std::string instance_v2 = web::http::client::http_client(getURL() + API_V2_INSTANCE)
            .request(web::http::methods::GET).get().extract_utf8string().get();
    if (instance_v2[0] == '[' || instance_v2[0] == '{') {
        instance = std::pair<nlohmann::json , api_version>(nlohmann::json::parse(instance_v2), mastodon::api_version::v2);
        return instance.value();
    } else {
        std::string instance_v1 = web::http::client::http_client(getURL() + API_V1_INSTANCE)
                .request(web::http::methods::GET).get().extract_utf8string().get();
        if (instance_v1[0] == '[' || instance_v1[0] == '{') {
            instance = std::pair<nlohmann::json, api_version>(nlohmann::json::parse(instance_v1),
                                                              mastodon::api_version::v1);
            return instance.value();
        }
    }
    throw std::runtime_error("cannot access api");
}

std::string mastodon::getIcon(){
    return "/favicon.ico";
}

std::string mastodon::getName() {
    return getInstance().first["title"].get<std::string>();
}

std::string mastodon::getServerVersion() {
    return getInstance().first["version"].get<std::string>();
}
