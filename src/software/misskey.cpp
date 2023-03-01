//
// Created by folosuru on 2023/02/24.
//

#include <future>
#include "../api.hpp"

#include "nlohmann/json.hpp"


class misskey : public api {

public:
    static const int instance_get_limit = 100;

    static const int access_thread_limit = 10;

    static const int connect_timeout = 5;

    using api::api;

    std::optional<instance_list> fetchAllFederation() override {
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
                    std::wcout << "timeout:" << this->getURL() << std::endl;
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

    int getFederationCount() override {
        if (this->FederationCount) {
            return this->FederationCount.value();
        } else {
            return web::http::client::http_client(getURL() + L"/api/stats")
                    .request(web::http::methods::POST, L"", L"{}", L"application/json")
                    .get()
                    .extract_json().get()[L"instances"].as_integer();
        }
    }

    static std::optional<instance_list> fetchFederation(const utility::string_t &URL, int offset) {
        std::vector<std::string> list;
        web::http::client::http_client client(URL + L"/api/federation/instances");
        web::json::value json;
        json[L"limit"] = instance_get_limit;
        json[L"offset"] = offset;
        auto response = client.request(web::http::methods::POST, L"", json.serialize(), L"application/json").get();
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

};