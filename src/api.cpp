//
// Created by folosuru on 2023/02/27.
//

#include "api.hpp"
#include "software/misskey.cpp"
#include "software/other.cpp"

api * api::getInstance(const utility::string_t& URL) {

    try {
        auto nodeinfo_url = web::http::client::http_client(L"https://" + URL + L"/.well-known/nodeinfo")
                .request(web::http::methods::GET).get()
                .extract_json().get()[L"links"][0][L"href"].as_string();

        nlohmann::json nodeinfo = nlohmann::json::parse(web::http::client::http_client(nodeinfo_url)
                .request(web::http::methods::GET).get()
                .extract_utf8string().get());

        auto software_name = nodeinfo["software"]["name"].get<std::string>();
        if (software_name == "misskey") return new misskey(URL, nodeinfo);
        return new other(URL, nodeinfo);
    } catch (web::http::http_exception& error) {
        return nullptr;
    }
}


utility::string_t api::getURL() {
    return L"https://" + URL;
}

api::api(const utility::string_t& URL, nlohmann::json nodeinfo) {
    this->URL = URL;
    this->nodeinfo = std::move(nodeinfo);
}
