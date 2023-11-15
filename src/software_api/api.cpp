//
// Created by folosuru on 2023/02/27.
//

#include "api.hpp"

#include <memory>
#include <utility>

#include "misskey.hpp"
#include "mastodon.hpp"
#include "pleroma.hpp"

std::shared_ptr<api> api::getInstance(const target_domain& target,
                                      const std::shared_ptr<util::blacklist>& blackList) {
    utility::string_t URL = utility::conversions::to_string_t(target.domain);
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

    if (software_name == "misskey") return std::shared_ptr<api>(new misskey(target, blackList, nodeinfo, manifest));
    if (software_name == "foundkey") return std::shared_ptr<api>(new misskey(target, blackList, nodeinfo, manifest));
    if (software_name == "meisskey") return std::shared_ptr<api>(new misskey(target, blackList, nodeinfo, manifest));
    if (software_name == "calckey") return std::shared_ptr<api>(new misskey(target, blackList, nodeinfo, manifest));

    if (software_name == "mastodon") return std::shared_ptr<api>(new mastodon(target, blackList, nodeinfo, manifest));
    if (software_name == "ecko") return std::shared_ptr<api>(new mastodon(target, blackList, nodeinfo, manifest));
    if (software_name == "Fedibird") return std::shared_ptr<api>(new mastodon(target, blackList, nodeinfo, manifest));
    if (software_name == "hometown") return std::shared_ptr<api>(new mastodon(target, blackList, nodeinfo, manifest));

    if (software_name == "pleroma") return std::shared_ptr<api>(new pleroma(target, blackList, nodeinfo, manifest));
    if (software_name == "akkoma") return std::shared_ptr<api>(new pleroma(target, blackList, nodeinfo, manifest));

    try {
        std::shared_ptr<mastodon> instance(new mastodon(target, blackList, nodeinfo, manifest));
        instance->getInstanceData();
        return instance;
    } catch (...) {}
    try {
        std::shared_ptr<misskey> instance(new misskey(target, blackList, nodeinfo, manifest));
        instance->getMeta();
        return instance;
    } catch (...) {
        return std::make_shared<api>(target, blackList, nodeinfo, manifest);
    }
}


utility::string_t api::getURL() {
    return HTTPS_URI_SCHEME + URL;
}

api::api(target_domain domain_,
         std::shared_ptr<util::blacklist> blackList,
         nlohmann::json nodeinfo,
         nlohmann::json manifest) : domain(std::move(domain_)) {
    this->blacklist_ = std::move(blackList);
    this->URL = utility::conversions::to_string_t(domain.domain);
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
    return manifest["name"].get<std::string>();
}

api::register_status api::getRegisterStatus() {
    return api::unknown;
}

std::string api::getBanner() {
    return "";
}

void api::fetchFederationToQueue() {}
