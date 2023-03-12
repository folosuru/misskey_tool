//
// Created by folosuru on 2023/03/12.
//
#pragma once
#include <string>

namespace util {
    std::string addScheme(const std::string& path , const std::string& domain) {
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
}