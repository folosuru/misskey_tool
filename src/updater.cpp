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
        util::sql::updateInstance(update_connection, api::getInstance(domain));
        std::cout << "update :" << domain << std::endl;
    }
    return 0;
}
