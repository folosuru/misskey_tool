//
// Created by folosuru on 2023/02/27.
//

#include "api.hpp"

#include <utility>

#include "misskey.hpp"
#include "mastodon.hpp"
#include "pleroma.hpp"

api * api::getInstance(const std::string& URL_) {
    utility::string_t URL = utility::conversions::to_string_t(URL_);
    web::http::client::http_client_config conf;
    auto nodeinfo_url = nlohmann::json::parse(web::http::client::http_client(HTTPS_URI_SCHEME + URL + NODEINFO_PATH , conf)
            .request(web::http::methods::GET).get().extract_utf8string().get())
            ["links"][0]["href"].get<std::string>();

    nlohmann::json nodeinfo = nlohmann::json::parse(
            web::http::client::http_client(utility::conversions::to_string_t(nodeinfo_url),conf)
            .request(web::http::methods::GET).get()
            .extract_utf8string().get()
            );
    nlohmann::json manifest;
    try {
        manifest = nlohmann::json::parse(
                web::http::client::http_client(HTTPS_URI_SCHEME + URL + MANIFEST_PATH, conf)
                        .request(web::http::methods::GET).get()
                        .extract_utf8string().get());
    } catch (...) {
        manifest = nlohmann::json();
    }
    auto software_name = nodeinfo["software"]["name"].is_string()
            ? nodeinfo["software"]["name"].get<std::string>()
            : "" ;

    if (software_name == "misskey") return new misskey(URL, nodeinfo, manifest);
    if (software_name == "foundkey") return new misskey(URL, nodeinfo, manifest);
    if (software_name == "meisskey") return new misskey(URL, nodeinfo, manifest);
    if (software_name == "calckey") return new misskey(URL, nodeinfo, manifest);

    if (software_name == "mastodon") return new mastodon(URL, nodeinfo, manifest);
    if (software_name == "ecko") return new mastodon(URL, nodeinfo, manifest);
    if (software_name == "Fedibird") return new mastodon(URL, nodeinfo, manifest);
    if (software_name == "hometown") return new mastodon(URL, nodeinfo, manifest);

    if (software_name == "pleroma") return new pleroma(URL, nodeinfo, manifest);
    if (software_name == "akkoma") return new pleroma(URL, nodeinfo, manifest);

    mastodon * instance;
    try {
        instance = new mastodon(URL, nodeinfo, manifest);
        instance->getInstance();
        return instance;
    } catch (...) {
        delete instance;
    }

    return new api(URL, nodeinfo, manifest);
}


utility::string_t api::getURL() {
    return HTTPS_URI_SCHEME + URL;
}

api::api(const utility::string_t& URL, nlohmann::json nodeinfo , nlohmann::json manifest) {
    this->URL = URL;
    this->nodeinfo = std::move(nodeinfo);
    this->manifest = std::move(manifest);
}


std::string api::getDescription() {
    return "";
}

std::string api::getServerSoftware() {
    return nodeinfo["software"]["name"].is_string()
        ? nodeinfo["software"]["name"].get<std::string>()
        : "";
}

int api::getUserCount() {
    return nodeinfo["usage"]["users"]["total"].is_number_integer()
        ? nodeinfo["usage"]["users"]["total"].get<int>()
        : 0;
}

int api::getPostsCount() {
    return nodeinfo["usage"]["localPosts"].is_number_integer()
        ? nodeinfo["usage"]["localPosts"].get<int>()
        : 0;
}

std::optional<api::instance_list> api::fetchAllFederation(){
    return std::nullopt;
}

int api::getFederationCount(){
    return 0;
}

std::string api::getIcon() {
    return manifest["icons"][0]["src"].is_string()
    ? manifest["icons"][0]["src"].get<std::string>()
    : "";
}

std::string api::getServerVersion() {
    return nodeinfo["software"]["version"].is_string()
    ? nodeinfo["software"]["version"].get<std::string>()
    : "";
}

std::string api::getDomain() {
    return utility::conversions::to_utf8string(URL);
}

const utility::string_t api::HTTPS_URI_SCHEME = utility::conversions::to_string_t("https://");
const utility::string_t api::NODEINFO_PATH = utility::conversions::to_string_t("/.well-known/nodeinfo");
const utility::string_t api::MANIFEST_PATH = utility::conversions::to_string_t("/manifest.json");
const utility::string_t api::LINKS = utility::conversions::to_string_t("links");
const utility::string_t api::HREF = utility::conversions::to_string_t("href");

std::string api::getName() {
    return manifest["name"].get<std::string>();;
}

api::register_status api::getRegisterStatus() {
    return api::unknown;
}
