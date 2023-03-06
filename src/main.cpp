﻿#include <iostream>
#include "api.hpp"
#include <pqxx/pqxx>
#include <sw/redis++/redis++.h>
#include <future>
#include <thread>
#include "util/sql.hpp"

using namespace sw::redis;

int main() {
    try{
        pqxx::connection c("user=misskey_tool password=test");
        std::cout << "Connected to " << c.dbname() << '\n';
        pqxx::work tx(c);
        tx.exec("create table IF NOT EXISTS instance_list ("
                " domain text ,"
                " user_count int,"
                " post_count int,"
                " software text,"
                " data text,"
                " federation_count int"
                " ,UNIQUE(domain));");
        tx.commit();
        pqxx::work createIndex(c);
        createIndex.exec("CREATE INDEX IF NOT EXISTS domain_name_index ON instance_list (domain);");
        createIndex.commit();
        c.close();
    } catch (std::exception const &e){
        std::cerr << "Database setup error: " << e.what() << std::endl;
        return 1;
    }
    auto redis = Redis("tcp://127.0.0.1:6379");

    /* first instance... */
    api * instance = api::getInstance("msky.z-n-a-k.net");
    auto list = instance->fetchAllFederation();
    if (list) {
        for (const auto &item: list.value()) {
            redis.set("misskey_tool:queue:" + item, "");
        }
    }


    std::vector<std::thread> thread_list;
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        thread_list.emplace_back(std::thread([] {

            std::cout << "start thread..." << std::endl;
            pqxx::connection db("user=misskey_tool password=test");
            auto redis = Redis("tcp://127.0.0.1:6379");
            while (true) {

                std::unordered_set<std::string> keys;
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
                std::string url = *keys.begin();
                url.erase(0, 19);
                try {

                    redis.rename("misskey_tool:queue:" + url, "misskey_tool:working:" + url);

                    std::cout << "get: " + url << std::endl;

                    auto i = api::getInstance(url);

                    if (redis.exists("misskey_tool:history:" + url)) {
                        if (redis.get("misskey_tool:history:" + url).value() == std::to_string(i->getFederationCount())) {
                            continue;
                        }
                    }
                    auto list = i->fetchAllFederation();
                    util::sql::writeInstance(db, url, i->getUserCount(), i->getPostsCount(), i->getServerSoftware(),
                                             i->getSummary(), i->getFederationCount());
                    redis.rename("misskey_tool:working:" + url, "misskey_tool:history:" + url);
                    if (!list) continue;
                    for (const auto &i1: list.value()) {
                        if (redis.exists("misskey_tool:*" + i1) || util::sql::isExistByDomain(db, i1)) {
                            continue;
                        } else {
                            redis.set("misskey_tool:queue:" + i1, "0");
                        }
                    }
                    std::cout << "complete: " + url << std::endl;
                } catch (sw::redis::Error &e) {
                    std::cerr << "Error: redis: " << e.what() << std::endl;
                    continue;
                } catch (web::http::http_exception& e){
                    std::cerr << "Error: http: " << url << " : " << e.what() << std::endl;
                    continue;
                } catch (nlohmann::json::exception& e){
                    std::cerr << "Error: json: " << url << " : " << e.what() << std::endl;
                    continue;
                } catch (std::exception& e){
                    std::cerr << "skipped: " << url << " : " << e.what() << std::endl;
                    continue;
                }
            }
        }));

    }
    for (auto& thread : thread_list) {
        thread.join();
    }

    return 0;
}

