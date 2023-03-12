//
// Created by folosuru on 2023/03/02.
//
#pragma once

#include <string>
#include <pqxx/pqxx>
#include "../api.hpp"


namespace util::sql {

    bool isExistByDomain(pqxx::connection &connection, const std::string &domain);

    void initDB();

    pqxx::connection createConnection();

    void writeInstance(pqxx::connection &connection,
                       std::string &domain,
                       int user_count,
                       int post_count,
                       std::string software,
                       std::string data,
                       int federation_count);

    void writeInstance(pqxx::connection &connection, api* api);
}