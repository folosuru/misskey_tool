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


    std::vector<std::thread> thread_list;

        thread_list.emplace_back(std::thread([]{
            auto redis = Redis("tcp://127.0.0.1:6379");
            std::unordered_set<std::string> keys;
            redis.scan( 0LL, "misskey_tool:queue*", 1 , std::inserter(keys, keys.begin()));
            std::string url = *keys.begin();
            url.erase(0, 19);
            redis.rename("misskey_tool:queue:" + url, "misskey_tool:history:" + url);
            std::cout << "get: " + url << std::endl;
            if (url == "misskey.io") {
                return;
            }
            auto i = api::getInstance(std::wstring(url.begin(), url.end()));
            if (i == nullptr) {
                std::cout << "nullptr" << std::endl;
                return;
            }
            auto list = i->fetchAllFederation();

            if (!list) return;
            for (const auto& i1: list.value()) {
                std::cout << i1 << std::endl;
                if (redis.get("misskey_tool:*" + i1)){
                    continue;
                } else {
                    redis.set("misskey_tool:queue:" + i1, "");
                }
            }
        }));


    for (auto& thread : thread_list) {
        thread.join();
    }

    return 0;
}


