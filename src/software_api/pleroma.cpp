//
// Created by folosuru on 2023/03/12.
//

#include "pleroma.hpp"

std::string pleroma::getDescription() {

    if (getInstance().second == pleroma::api_version::v2){
        return getInstance().first["description"].get<std::string>();
    }

    if (getInstance().second == pleroma::api_version::v1){
        return getInstance().first["short_description"].get<std::string>();
    }
    return "";
}

const utility::string_t pleroma::API_V1_INSTANCE = utility::conversions::to_string_t("/api/v1/instance");
const utility::string_t pleroma::API_V2_INSTANCE = utility::conversions::to_string_t("/api/v2/instance");
const utility::string_t pleroma::API_PEERS = utility::conversions::to_string_t("/api/v1/instance/peers");


std::optional<api::instance_list> pleroma::fetchAllFederation() {
    return getPeers();
}

std::optional<api::instance_list> pleroma::getPeers() {
    if (peers){
        return peers.value();
    }
    peers = api::instance_list();
    nlohmann::json json;
    try {
        json = nlohmann::json::parse(
                web::http::client::http_client(getURL() + API_PEERS)
                        .request(web::http::methods::GET).get().extract_utf8string().get()
        );
    } catch (...){
        return std::nullopt;
    }
    peers.value() = instance_list();
    for (auto& i : json){
        peers.value()->push_back(i.get<std::string>());
    }
    return peers.value();
}

int pleroma::getFederationCount() {
    if (getPeers()) {
        return static_cast<int>(getPeers().value().size());
    } else {
        return 0;
    }
}

std::pair<nlohmann::json, pleroma::api_version> pleroma::getInstance() {
    if (instance) {
        return instance.value();
    }

    std::string instance_v2 = web::http::client::http_client(getURL() + API_V2_INSTANCE)
            .request(web::http::methods::GET).get().extract_utf8string().get();
    if (instance_v2[0] == '[' || instance_v2[0] == '{') {
        instance = std::pair<nlohmann::json, api_version>(nlohmann::json::parse(instance_v2), pleroma::api_version::v2);
        if (!instance.value().first["version"].is_null()) {
            return instance.value();
        }
    }
    std::string instance_v1 = web::http::client::http_client(getURL() + API_V1_INSTANCE)
            .request(web::http::methods::GET).get().extract_utf8string().get();
    if (instance_v1[0] == '[' || instance_v1[0] == '{') {
        instance = std::pair<nlohmann::json, api_version>(nlohmann::json::parse(instance_v1),
                                                          pleroma::api_version::v1);
        if (!instance.value().first["version"].is_null()) {
            return instance.value();
        }
    }
    throw std::runtime_error("cannot access api");
}

std::string pleroma::getIcon(){
    return "/favicon.png";
}

std::string pleroma::getName() {
    return getInstance().first["title"].get<std::string>();
}

std::string pleroma::getServerVersion() {
    return getInstance().first["version"].get<std::string>();
}

std::string pleroma::getBanner() {
    if (getInstance().second == api_version::v2){
        return getInstance().first["thumbnail"]["url"].get<std::string>();
    } else {
        return getInstance().first["thumbnail"].get<std::string>();
    }
}

void pleroma::fetchFederationToQueue() {
    std::optional<instance_list> list = fetchAllFederation();
    if (list) {
        for (const auto& federation_instance : list.value()) {
            if (!blacklist_->isBlacklisted(federation_instance)) {
                domain.addQueue(federation_instance);
                //std::cout << "get:" << federation_instance << std::endl;
            }
        }
    }
}
