//
// Created by folosuru on 2023/02/27.
//

#include "api.hpp"

#include <utility>

#include "software/misskey.hpp"

api * api::getInstance(const std::string& URL_) {
    utility::string_t URL = utility::conversions::to_string_t(URL_);
    try {
        try {
            auto nodeinfo_url = web::http::client::http_client( HTTP_URI_SCHEME + URL + NODEINFO_PATH)
                    .request(web::http::methods::GET).get()
                    .extract_json().get()[LINKS][0][HREF].as_string();

            nlohmann::json nodeinfo = nlohmann::json::parse(web::http::client::http_client(nodeinfo_url)
                                                                    .request(web::http::methods::GET).get()
                                                                    .extract_utf8string().get());

            nlohmann::json manifest = nlohmann::json::parse(
                    web::http::client::http_client(HTTP_URI_SCHEME + URL + MANIFEST_PATH)
                            .request(web::http::methods::GET).get()
                            .extract_utf8string().get());

            auto software_name = nodeinfo["software"]["name"].get<std::string>();

            if (software_name == "misskey") return new misskey(URL, nodeinfo, manifest);

            return new api(URL, nodeinfo, manifest);
        } catch (std::exception& exception) {
            std::cerr << URL_ << ":" << exception.what() << std::endl;
            return nullptr;
        }
    } catch (web::http::http_exception& error) {
        std::cerr << URL_ << ":" << error.what() << std::endl;
        return nullptr;
    }
}


utility::string_t api::getURL() {
    return HTTP_URI_SCHEME + URL;
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

std::optional<api::instance_list> api::fetchAllFederation(){
    return std::nullopt;
}

int api::getFederationCount(){
    return 0;
}

const utility::string_t api::HTTP_URI_SCHEME = utility::conversions::to_string_t("https://");
const utility::string_t api::NODEINFO_PATH = utility::conversions::to_string_t("/.well-known/nodeinfo");
const utility::string_t api::MANIFEST_PATH = utility::conversions::to_string_t("/manifest.json");
const utility::string_t api::LINKS = utility::conversions::to_string_t("kinks");
const utility::string_t api::HREF = utility::conversions::to_string_t("href");