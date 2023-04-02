//
// Created by folosuru on 2023/03/14.
//
#include "util.hpp"

std::string util::addScheme(const std::string& path , const std::string& domain) {
    if (path.substr(0,4) == "http"){
        return path;
    } else {
        if (path[0] == '/') {
            return "https://" + domain + path;
        } else {
            return "https://" + domain + "/" + path;
        }
    }
}

std::string util::getTopLevelDomain(const std::string &domain) {
    std::vector<std::string> v;
    std::stringstream ss(domain);
    std::string buffer;
    while( std::getline(ss, buffer, '.') ) {
        v.push_back(buffer);
    }
    if (v.size() == 2){
        return domain;
    }
    return v[v.size()-2]+'.'+v[v.size()-1];
}
