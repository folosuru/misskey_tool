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

}