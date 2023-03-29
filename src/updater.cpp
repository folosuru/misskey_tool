//
// Created by folosuru on 2023/03/27.
//
#include "util/sql.hpp"
#include "software_api/api.hpp"
#include <pqxx/pqxx>

int main() {
    pqxx::connection connection = util::sql::createConnection();
    pqxx::connection update_connection = util::sql::createConnection();
    pqxx::work tx(connection);
    for (auto [domain] : tx.query<std::string>(
            "SELECT domain FROM instance_list;"))
    {
        api * instance;
        try {
            instance = api::getInstance(domain);
        } catch (...) { continue; }
        try {
            util::sql::updateInstance(update_connection, instance);
            std::cout << "update :" << domain << "\n";
        } catch (...) {
            delete instance;
            continue;
        }
    }
    return 0;
}
