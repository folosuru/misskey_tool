//
// Created by folosuru on 2023/03/01.
//

#include <pqxx/pqxx>

namespace util::sql{

    bool isExistByDomain(pqxx::connection& connection , const std::string& domain) {
        pqxx::work work(connection);
        auto result = work.query1<bool>("SELECT EXISTS (select * from instance_list where domain ="+ work.quote(domain) +")");
        return std::get<0>(result);
    }

    void writeInstance(pqxx::connection& connection ,
                       std::string& domain ,
                       int user_count ,
                       int post_count ,
                       std::string software ,
                       std::string data ,
                       int federation_count){
        pqxx::work work(connection);
        work.exec("insert into instance_list (domain, user_count, post_count, software, data, federation_count)"
                  "  VALUES (" + work.quote(domain) + "," + std::to_string(user_count) + "," +std::to_string(post_count) + ","
                  + work.quote(software) + " , " + work.quote(data) + "," + std::to_string(federation_count) + ")");
        work.commit();
    }

}