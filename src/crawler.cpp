#include <iostream>
#include "software_api/api.hpp"
#include <pqxx/pqxx>
#include <sw/redis++/redis++.h>
#include <thread>
#include "util/util.hpp"
#include "util/sql.hpp"
#include "util/blacklist.hpp"

using namespace sw::redis;

int main() {
    try{
        util::sql::initDB();
    } catch (std::exception const &e){
        std::cerr << "Database setup error: " << e.what() << std::endl;
        return 1;
    }
    auto redis = Redis("tcp://127.0.0.1:6379");

    /* first instance...
     *
     * you can change url to another instance.
     */
    std::unordered_set<std::string> keys;
    auto cursor = 0LL;
    cursor = redis.scan(cursor, "misskey_tool:queue*", 1, std::inserter(keys, keys.begin()));
    if (cursor == 0) {
        api * instance = api::getInstance("msky.z-n-a-k.net");
        auto list = instance->fetchAllFederation();
        if (list) {
            for (const auto &item: list.value()) {
                redis.set("misskey_tool:queue:" + item, "");
            }
        }
    }


    std::vector<std::thread> thread_list;
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        thread_list.emplace_back(std::thread([] {
            std::cout << "start thread..." << std::endl;
            pqxx::connection db = util::sql::createConnection();
            auto redis = Redis("tcp://127.0.0.1:6379");
            while (true) {

                std::unordered_set<std::string> keys;
                keys.clear();
                try {
                    auto cursor = 0LL;
                    while (true) {
                        cursor = redis.scan(cursor, "misskey_tool:queue*", 1, std::inserter(keys, keys.begin()));
                        if (keys.empty()) {
                            if (cursor == 0) {
                                std::cout << "empty" << std::endl;
                                return;
                            }
                            continue;
                        } else {
                            break;
                        }
                    }
                } catch (std::exception& exception){
                    std::cout << "Error in get key: " << exception.what() << std::endl;
                    continue;
                }
                std::string url = *keys.begin();
                url.erase(0, 19);
                try {
                    redis.rename("misskey_tool:queue:" + url, "misskey_tool:working:" + url);
                } catch (sw::redis::ReplyError &e) {
                    continue;
                }
                if (util::blacklist::isBlacklisted(db, url)){
                    redis.del("misskey_tool:working:" + url);
                    std::cout << "skip: blacklist:" + url << std::endl;
                    continue;
                }
                std::cout << "get: " + url << std::endl;
                api* i;
                try {
                    i = api::getInstance(url);
                } catch (std::exception &exception) {
                    std::cout << "Error: Cannot access resource: " + url << std::endl;
                    try {
                        redis.rename("misskey_tool:working:" + url, "misskey_tool:fail:" + url);
                        std::string topleveldomain = util::getTopLevelDomain(url);
                        long long fail_score = redis.incr("misskey_tool:fail_domain:"+topleveldomain);
                        if (fail_score > 10){
                            util::sql::addBlacklist(db,topleveldomain);
                            std::cout << "add blacklist:" + topleveldomain <<std::endl;
                        }
                    } catch (sw::redis::ReplyError &e) {}
                    continue;
                }
                try {
                    if (redis.get("misskey_tool:working:" + url).value() ==
                        std::to_string(i->getFederationCount()) && i->getFederationCount() != 0 ) {
                        redis.rename("misskey_tool:working:" + url, "misskey_tool:history:" + url);
                        delete i;
                        continue;
                    }
                    std::optional<api::instance_list> list = i->fetchAllFederation();
                    util::sql::writeInstance(db, i);
                    std::cout << "wrote: " + url << std::endl;
                    redis.rename("misskey_tool:working:" + url, "misskey_tool:history:" + url);
                    redis.set("misskey_tool:history:" + url , std::to_string(i->getFederationCount()));
                    if (!list) {
                        delete i;
                        continue;
                    }
                    for (const auto &i1: list.value()) {
                        if (redis.exists("misskey_tool:*" + i1) || util::sql::isExistByDomain(db, i1)) {
                            continue;
                        } else {
                            if (util::blacklist::isBlacklisted(db,i1)) {
                                continue;
                            }
                            redis.set("misskey_tool:queue:" + i1, "0");
                        }
                    }
                    list.value().clear();
                    std::cout << "complete: " + url << std::endl;
                } catch (sw::redis::Error &e) {
                    std::cerr << "Error: redis: " << e.what() << std::endl;
                } catch (std::exception& e) {
                    if (typeid(e) == typeid(web::http::http_exception)) {
                        std::cerr << "Error: http: " << url << " : " << e.what() << std::endl;
                    } else if (typeid(e) == typeid(nlohmann::json::exception)) {
                        std::cerr << "Error: json: " << url << " : " << e.what() << std::endl;
                    } else {
                        std::cerr << "Error: other: " << url << " : " << e.what() << std::endl;
                    }
                    try{
                        redis.rename("misskey_tool:working:" + url, "misskey_tool:fail:" + url);
                    } catch (sw::redis::ReplyError &error) {}
                }
                delete i;
            }
        }));

    }
    for (auto& thread : thread_list) {
        thread.join();
    }

    return 0;
}

