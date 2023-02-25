//
// Created by folosuru on 2023/02/24.
//

#include "misskey.hpp"

std::optional<std::vector<std::string>> misskey::fetchFederation(const utility::string_t &URL , int offset) {
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



