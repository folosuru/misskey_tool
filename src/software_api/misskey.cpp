//
// Created by folosuru on 2023/02/24.
//
#include "misskey.hpp"

const int misskey::instance_get_limit = 100;


std::optional<api::instance_list> misskey::fetchAllFederation() {
    return std::nullopt;
}

int misskey::getFederationCount() {
    if (this->FederationCount) {
        return this->FederationCount.value();
    } else {
        return web::http::client::http_client(getURL() + STATS_PASS)
                .request(web::http::methods::POST, "", "{}", "application/json")
                .get()
                .extract_json().get()[INSTANCES].as_integer();
    }
}

std::optional<api::instance_list> misskey::fetchFederation(const utility::string_t &URL, int offset) {
    try {
        instance_list list;
        web::http::client::http_client client(URL + INSTANCES_PATH);
        web::json::value json;
        json[LIMIT] = instance_get_limit;
        json[OFFSET] = offset;
        auto response = client.request(web::http::methods::POST, utility::string_t(), json.serialize(),
                                       MIME_APPLICATION_JSON).get();
        std::string data = response.extract_utf8string().get();

        if (!(data[0] == '[' || data[0] == '{')) {
            return std::nullopt;
        }
        nlohmann::json instances = nlohmann::json::parse(data);
        for (auto item: instances) {
            if (item["host"].is_string()) {
                list.push_back(item["host"].get<std::string>());
            }
        }
        return list;
    } catch (std::exception& e) {
        return std::nullopt;
    }
}

std::string misskey::getDescription() {
    if (nodeinfo["metadata"]["nodeDescription"].is_null()) {
        return "";
    } else {
        return nodeinfo["metadata"]["nodeDescription"].get<std::string>();
    }
}
api::register_status misskey::getRegisterStatus() {
    try {
        bool registration = getMeta()["features"]["registration"].get<bool>();
        if (registration) {
            return api::register_status::everyone;
        } else {
            return api::register_status::invite;
        }
    } catch (std::exception& e){
        return api::register_status::unknown;
    }
}

const utility::string_t misskey::INSTANCES_PATH = utility::conversions::to_string_t("/api/federation/instances");
const utility::string_t misskey::MIME_APPLICATION_JSON = utility::conversions::to_string_t("application/json");
const utility::string_t misskey::STATS_PASS = utility::conversions::to_string_t("/api/stats");
const utility::string_t misskey::LIMIT = utility::conversions::to_string_t("limit");
const utility::string_t misskey::OFFSET = utility::conversions::to_string_t("offset");
const utility::string_t misskey::INSTANCES = utility::conversions::to_string_t("instances");
const utility::string_t misskey::LOG_TIMEOUT = utility::conversions::to_string_t("timeout:");
const utility::string_t misskey::META_PASS = utility::conversions::to_string_t("/api/meta");

nlohmann::json misskey::getMeta() {
    if (!meta) {
        meta = nlohmann::json::parse(web::http::client::http_client(getURL() + META_PASS)
                                      .request(web::http::methods::POST, "", "{}", "application/json")
                                      .get().extract_utf8string().get());
    }
    return meta.value();
}

std::string misskey::getIcon() {
    if (!getMeta()["iconUrl"].is_null()) {
        if (getMeta()["iconUrl"].get<std::string>().empty()){
            return "/favicon.ico";
        }
        return getMeta()["iconUrl"].get<std::string>();
    } else {
        return "/favicon.ico";
    }
}

std::string misskey::getName() {
    return getMeta()["name"].get<std::string>();
}

std::string misskey::getBanner() {
    if (!getMeta()["bannerUrl"].is_null()){
        return  getMeta()["bannerUrl"].get<std::string>();
    } else {
        return "";
    }
}

void misskey::fetchFederationToQueue() {
    for (int i = 0; i < (getFederationCount() / instance_get_limit) + 1; ++i) {
        std::optional<instance_list> list = fetchFederation(getURL(), i*instance_get_limit);
        if (list) {
            for (const auto& federation_instance : list.value()) {
                if (!blacklist_->isBlacklisted(federation_instance)) {
                    domain.addQueue(federation_instance);
                    //std::cout << "get:" << federation_instance << std::endl;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

