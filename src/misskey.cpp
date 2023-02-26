//
// Created by folosuru on 2023/02/24.
//

#include <future>
#include "server_api.hpp"

using namespace api;

std::optional<instance_list> misskey::fetchAllFederation(const utility::string_t &URL) {
    // get federation from /api/stats .
    std::string stats = web::http::client::http_client(URL + L"/api/stats")
        .request(web::http::methods::POST, L"" , L"{}" , L"application/json")
        .get()
        .extract_utf8string().get();

    if (!(stats[0] == '[' | stats[0] == '{')) return std::nullopt;

    int instance_count = nlohmann::json::parse(stats)["instances"].get<int>();

    int i= 0;
    int offset = 0;
    instance_list list;
    while ( instance_count >= i * access_thread_limit * instance_get_limit) {
        std::vector<std::future<std::optional<instance_list>>> future_list;
        std::vector<std::thread> thread_list;
        for (int j = 0;  j < access_thread_limit & (j + i * access_thread_limit ) * instance_get_limit < instance_count; ++j) {
            offset = (j + i * access_thread_limit ) * instance_get_limit;
            std::promise<std::optional<instance_list>> promise;
            future_list.push_back(promise.get_future());
            thread_list.emplace_back([URL,offset](std::promise<std::optional<instance_list>> promise){
                promise.set_value(misskey::fetchFederation(URL,offset));
            } , std::move(promise));
        }

        std::chrono::time_point start_waiting = std::chrono::steady_clock::now();

        for (auto &future : future_list ){
            if (future.wait_until(start_waiting + std::chrono::seconds(connect_timeout)) != std::future_status::ready) return std::nullopt;
            std::optional<instance_list> data = future.get();
            if (!data) return std::nullopt;
            list.insert(list.end(), data.value().begin() , data.value().end() );
        }
        for (auto &t :thread_list){
            t.join();
        }
        ++i;
    }
    return list;
}

std::optional<instance_list> misskey::fetchFederation(const utility::string_t &URL , int offset) {
    std::vector<std::string> list;
    web::http::client::http_client client(URL + L"/api/federation/instances");
    web::json::value json;
    json[L"limit"] = instance_get_limit;
    json[L"offset"] = offset;
    auto response = client.request(web::http::methods::POST, L"", json.serialize() , L"application/json").get();
    std::string data = response.extract_utf8string().get();

    if (!(data[0] == '[' | data[0] == '{')) {
        return std::nullopt;
    }
    nlohmann::json instances = nlohmann::json::parse(data);
    for (auto item : instances) {
        std::cout << "thread : | offset :" << offset << " | result : " << item["host"].get<std::string>() << std::endl;
        list.push_back(item["host"].get<std::string>());
    }
    return list;
}

