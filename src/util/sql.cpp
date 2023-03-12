//
// Created by folosuru on 2023/03/01.
//

#include "sql.hpp"
#include <iostream>

bool util::sql::isExistByDomain(pqxx::connection& connection , const std::string& domain) {
    pqxx::work work(connection);
    auto result = work.query1<bool>("SELECT EXISTS (select * from instance_list where domain ="+ work.quote(domain) +")");
    return std::get<0>(result);
}


void util::sql::writeInstance(pqxx::connection& connection, api* api) {
    pqxx::work work(connection);
    work.exec("insert into instance_list "
              "(domain, user_count, post_count, software, federation_count, description, icon, server_version, name, register, language) "
              "VALUES("
              + work.quote(utility::conversions::to_utf8string(api->getDomain())) + ","
              + work.quote(api->getUserCount()) + " , "
              + work.quote(api->getPostsCount()) + " , "
              + work.quote(api->getServerSoftware()) + " , "
              + work.quote(api->getFederationCount()) + " , "
              + work.quote(api->getDescription()) + " , "
              + work.quote(api->getIcon()) + " , "
              + work.quote(api->getServerVersion()) + " , "
              + work.quote(api->getName()) + " , "
              + work.quote(static_cast<int>(api->getRegisterStatus())) + " , "
              + "null" + " );"
            );
    work.commit();
}

void util::sql::initDB() {
    pqxx::connection c = createConnection();
    pqxx::work tx(c);
    tx.exec("create table IF NOT EXISTS instance_list ("
            " domain text ,"
            " user_count int,"
            " post_count int,"
            " software text,"
            " federation_count int,"
            " description text,"
            " icon text,"
            " server_version text,"
            " name text,"
            " register text,"
            " language text,"
            " UNIQUE(domain));");
    tx.commit();
    c.close();
}

pqxx::connection util::sql::createConnection() {
    return pqxx::connection("user=misskey_tool password=test");
}
