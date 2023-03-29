//
// Created by folosuru on 2023/03/27.
//
#include <pqxx/pqxx>
#include <sw/redis++/redis++.h>
#include <iostream>
using namespace sw::redis;

int main() {
    auto redis = Redis("tcp://127.0.0.1:6379");

    pqxx::connection connection = pqxx::connection("user=misskey_tool password=test");
    pqxx::work tx(connection);
    for (auto [domain , federation_count] : tx.query<std::string, int>(
            "SELECT domain, federation_count FROM instance_list;"))
    {
        try {
            redis.set("misskey_tool:queue:" + domain, std::to_string(federation_count));
            std::cout << "queue set :" << domain << "\n";
        } catch (...) { continue; }
    }
    return 0;
}
