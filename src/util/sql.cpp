//
// Created by folosuru on 2023/03/01.
//

#include "sql.hpp"
#include "util.hpp"

bool util::sql::isExistByDomain(pqxx::connection& connection , const std::string& domain) {
    pqxx::work work(connection);
    auto result = work.query1<bool>("SELECT EXISTS (select * from instance_list where domain ="+ work.quote(domain) +")");
    return std::get<0>(result);
}


void util::sql::writeInstance(pqxx::connection& connection, const std::shared_ptr<api>& api) {
    pqxx::work work(connection);
    work.exec("insert into instance_list "
              "(domain, user_count, post_count, software, federation_count, description, icon, server_version, name, register, banner, language, update_time) "
              "VALUES("
              + work.quote(api->getDomain()) + ","
              + work.quote(api->getUserCount()) + " , "
              + work.quote(api->getPostsCount()) + " , "
              + work.quote(api->getServerSoftware()) + " , "
              + work.quote(api->getFederationCount()) + " , "
              + work.quote(api->getDescription()) + " , "
              + work.quote(util::addScheme(api->getIcon() , api->getDomain())) + " , "
              + work.quote(api->getServerVersion()) + " , "
              + work.quote(api->getName()) + " , "
              + work.quote(static_cast<int>(api->getRegisterStatus())) + " , "
              + work.quote(util::addScheme(api->getBanner(), api->getDomain())) + " , "
              + "null" + ", 'now' )"
              + "ON CONFLICT ON CONSTRAINT instance_list_domain_key DO UPDATE SET "
                "user_count = " + work.quote(api->getUserCount()) + " , "
                "post_count = " + work.quote(api->getPostsCount()) + " , "
                "software = " + work.quote(api->getServerSoftware()) + " , "
                "federation_count = " + work.quote(api->getFederationCount()) + " , "
                "description = " + work.quote(api->getDescription()) + " , "
                "icon = " + work.quote(util::addScheme(api->getIcon() , api->getDomain())) + " , "
                "server_version = " + work.quote(api->getServerVersion()) + " , "
                "name = " + work.quote(api->getName()) + " , "
                "register = " + work.quote(static_cast<int>(api->getRegisterStatus())) + " , "
                "banner = " + work.quote(util::addScheme(api->getBanner(), api->getDomain())) + ","
                "update_time = 'now'"
            );
    work.commit();
}

void util::sql::initDB(pqxx::connection& c) {
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
            " banner text,"
            " language text,"
            "update_time timestamp,"
            " UNIQUE(domain));"
            "create table IF NOT EXISTS blacklist (domain text, UNIQUE(domain));"

            );
    tx.commit();
}

pqxx::connection util::sql::createConnection() {
    return pqxx::connection("user=misskey_tool password=test dbname=misskey_tool");
}

void util::sql::updateInstance(pqxx::connection &connection, api *api) {
    pqxx::work work(connection);
    work.exec("update instance_list set "
              "user_count = " + work.quote(api->getUserCount()) + " , "
              "post_count = " + work.quote(api->getPostsCount()) + " , "
              "software = " + work.quote(api->getServerSoftware()) + " , "
              "federation_count = " + work.quote(api->getFederationCount()) + " , "
              "description = " + work.quote(api->getDescription()) + " , "
              "icon = " + work.quote(util::addScheme(api->getIcon() , api->getDomain())) + " , "
              "server_version = " + work.quote(api->getServerVersion()) + " , "
              "name = " + work.quote(api->getName()) + " , "
              "register = " + work.quote(static_cast<int>(api->getRegisterStatus())) + " , "
              "banner = " + work.quote(api->getBanner()) +
              "where domain = " + work.quote(api->getDomain()) + ";"
    );
    work.commit();
}

void util::sql::addBlacklist(pqxx::connection &connection, const std::string& domain) {
    pqxx::work work(connection);
    work.exec("insert into blacklist values(" + work.quote(domain) + ") ON CONFLICT DO NOTHING;");
    work.commit();
}

bool util::sql::isExistInBlacklist(pqxx::connection &connection, const std::string& domain) {
    pqxx::work work(connection);
    auto result = work.query1<bool>("SELECT EXISTS (select * from blacklist where domain ="+ work.quote(domain) +")");
    return std::get<0>(result);
}
