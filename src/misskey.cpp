//
// Created by folosuru on 2023/02/24.
//

#include "misskey.hpp"

std::string misskey::fetchFederation(const utility::string_t &URL) {
    web::http::client::http_client client(URL + L"/api/federation/instances");
    auto response = client.request(web::http::methods::POST, L"", L"{}" , L"application/json").get();
    auto body = response.extract_utf8string().get();
    return body;
}