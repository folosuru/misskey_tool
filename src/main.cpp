#include <iostream>
#include "api.hpp"

#include <sw/redis++/redis++.h>
#include <future>

using namespace sw::redis;

int main() {

    auto redis = Redis("tcp://127.0.0.1:6379");

    api * instance = api::getInstance(L"msky.z-n-a-k.net");
    auto list = instance->fetchAllFederation();
    if (list) {
        for (const auto &item: list.value()) {
            redis.set("misskey_tool:queue:" + item, "");
        }
    }

    auto count = 5;
    std::unordered_set<std::string> keys;
    redis.scan( 0LL, "misskey_tool:queue*", count, std::inserter(keys, keys.begin()));

    std::vector<std::thread> thread_list;
    std::vector<std::future<std::optional<api::instance_list>>> result_list;
    for (const auto& item : keys) {
        std::promise<std::optional<api::instance_list>> promise;
        result_list.push_back(promise.get_future());
        thread_list.emplace_back([](std::string url , std::promise<std::optional<api::instance_list>> promise) {
            std::cout << "get: " + url << std::endl;
                url.erase(0, 19);
                if (url == "misskey.io") {
                    promise.set_value(std::nullopt);
                    return ;
                }
                auto i = api::getInstance(std::wstring(url.begin(), url.end()));
                if (i == nullptr) {
                    std::cout << "nullptr" << std::endl;
                    promise.set_value(std::nullopt);
                    return;
                }
                promise.set_value(i->fetchAllFederation());
            } , item , std::move(promise)
        );
    }

    for (auto& thread : thread_list) {
        thread.join();
    }
    for (auto& future : result_list ){
        auto result = future.get();
        if (result) {
            for (const auto& i: result.value()) {
                if (redis.get(i)){
                    continue;
                } else {
                    redis.set("misskey_tool:queue:" + i,"");
                }
            }
        }
    }
    return 0;
}


