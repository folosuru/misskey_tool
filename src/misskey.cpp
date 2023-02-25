//
// Created by folosuru on 2023/02/24.
//

#include "server_api.hpp"

using namespace api;

std::optional<instance_list> misskey::fetchAllFederation(const utility::string_t &URL) {
    // /api/statsから連合の数を取得、その後なんかいい感じに
    std::string stats = web::http::client::http_client(URL + L"/api/stats")
        .request(web::http::methods::POST, L"" , L"{}" , L"application/json")
        .get()
        .extract_utf8string().get();

    if (!(stats[0] == '[' | stats[0] == '{')) return std::nullopt;

    int instance_count = nlohmann::json(stats)["instances"].get<int>();

    static_cast<int>(instance_count / instance_get_limit);

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
        list.push_back(item["host"].get<std::string>());
    }
    return list;
}



