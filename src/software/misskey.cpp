//
// Created by folosuru on 2023/02/24.
//
#include "misskey.hpp"

const int misskey::instance_get_limit = 100;
const int misskey::access_thread_limit = 10;
const int misskey::connect_timeout = 5;

std::optional<api::instance_list> misskey::fetchAllFederation() {
    // get federation from /api/stats .
    int instance_count = getFederationCount();
    int i = 0;
    int offset = 0;
    instance_list list;
    while (instance_count >= i * access_thread_limit * instance_get_limit) {
        std::vector<std::future<std::optional<instance_list>>> future_list;
        std::vector<std::thread> thread_list;
        for (int j = 0;
             j < access_thread_limit & (j + i * access_thread_limit) * instance_get_limit < instance_count; ++j) {
            offset = (j + i * access_thread_limit) * instance_get_limit;
            std::promise<std::optional<instance_list>> promise;
            future_list.push_back(promise.get_future());
            thread_list.emplace_back([offset](const utility::string_t& URL , std::promise<std::optional<instance_list>> promise) {
                promise.set_value(fetchFederation(URL, offset));
            }, getURL() , std::move(promise));
        }

        std::chrono::time_point start_waiting = std::chrono::steady_clock::now();

        for (auto &future: future_list) {
            if (future.wait_until(start_waiting + std::chrono::seconds(connect_timeout)) != std::future_status::ready) {
                ucout << LOG_TIMEOUT << this->getURL() << std::endl;
                return std::nullopt;
            }
            std::optional<instance_list> data = future.get();
            if (!data) return std::nullopt;
            list.insert(list.end(), data.value().begin(), data.value().end());
        }
        for (auto &t: thread_list) {
            t.join();
        }
        ++i;
    }
    return list;
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
    std::vector<std::string> list;
    web::http::client::http_client client(URL + INSTANCES_PATH);
    web::json::value json;
    json[LIMIT] = instance_get_limit;
    json[OFFSET] = offset;
    auto response = client.request(web::http::methods::POST, utility::string_t() , json.serialize(), MIME_APPLICATION_JSON).get();
    std::string data = response.extract_utf8string().get();

    if (!(data[0] == '[' || data[0] == '{')) {
        return std::nullopt;
    }
    nlohmann::json instances = nlohmann::json::parse(data);
    for (auto item: instances) {
        list.push_back(item["host"].get<std::string>());
    }
    return list;
}

std::string misskey::getDescription() {
    if (nodeinfo["metadata"]["nodeDescription"].is_null()) {
        return "";
    } else {
        return nodeinfo["metadata"]["nodeDescription"].get<std::string>();
    }
}

const utility::string_t misskey::INSTANCES_PATH = utility::conversions::to_string_t("/api/federation/instances");
const utility::string_t misskey::MIME_APPLICATION_JSON = utility::conversions::to_string_t("application/json");
const utility::string_t misskey::STATS_PASS = utility::conversions::to_string_t("/api/stats");
const utility::string_t misskey::LIMIT = utility::conversions::to_string_t("limit");
const utility::string_t misskey::OFFSET = utility::conversions::to_string_t("offset");
const utility::string_t misskey::INSTANCES = utility::conversions::to_string_t("instances");
const utility::string_t misskey::LOG_TIMEOUT = utility::conversions::to_string_t("timeout:");
