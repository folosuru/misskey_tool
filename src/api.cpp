//
// Created by folosuru on 2023/02/27.
//

#include "api.hpp"

#include <utility>

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

        nlohmann::json manifest = nlohmann::json::parse(web::http::client::http_client(L"https://" + URL + L"/manifest.json")
                .request(web::http::methods::GET).get()
                .extract_utf8string().get());

        auto software_name = nodeinfo["software"]["name"].get<std::string>();

        if (software_name == "misskey") return new misskey(URL, nodeinfo,manifest);

        return new other(URL, nodeinfo , manifest);

    } catch (web::http::http_exception& error) {
        return nullptr;
    }
}


utility::string_t api::getURL() {
    return L"https://" + URL;
}

api::api(const utility::string_t& URL, nlohmann::json nodeinfo , nlohmann::json manifest) {
    this->URL = URL;
    this->nodeinfo = std::move(nodeinfo);
    this->manifest = std::move(manifest);
}

std::string api::getSummary() {
    nlohmann::json data;
    data["name"] = manifest["name"].get<std::string>();
    data["icon"] = manifest["icons"][0]["src"].get<std::string>();
    data["description"] = this->getDescription();
    data["server_version"] = nodeinfo["software"]["version"].get<std::string>();
    return data.dump();
}

std::string api::getDescription() {
    return "";
}

std::string api::getServerSoftware() {
    return nodeinfo["software"]["name"].get<std::string>();
}

int api::getUserCount() {
    return nodeinfo["usage"]["users"]["total"].get<int>();
}

int api::getPostsCount() {
    return nodeinfo["usage"]["localPosts"].get<int>();
}