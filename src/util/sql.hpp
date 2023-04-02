//
// Created by folosuru on 2023/03/02.
//
#pragma once

#include <string>
#include <pqxx/pqxx>
#include "../software_api/api.hpp"


namespace util::sql {

    bool isExistByDomain(pqxx::connection &connection, const std::string &domain);

    void initDB();

    pqxx::connection createConnection();

    bool isExistInBlacklist(pqxx::connection &connection,const std::string& domain);

    void addBlacklist(pqxx::connection &connection, const std::string& domain);

    void writeInstance(pqxx::connection &connection, api* api);

    void updateInstance(pqxx::connection &connection, api* api);
}