//
// Created by folosuru on 2023/03/12.
//
#pragma once
#include <string>
#include <sstream>
#include <vector>

namespace util {
    std::string addScheme(const std::string& path , const std::string& domain);

    std::string getTopLevelDomain(const std::string& domain);
}