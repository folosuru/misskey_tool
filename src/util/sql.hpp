//
// Created by folosuru on 2023/03/02.
//
#pragma once

#include <string>
#include <pqxx/pqxx>


namespace util::sql {
    bool isExistByDomain(pqxx::connection &connection, const std::string &domain);

    void writeInstance(pqxx::connection &connection,
                       std::string &domain,
                       int user_count,
                       int post_count,
                       std::string software,
                       std::string data,
                       int federation_count);
}