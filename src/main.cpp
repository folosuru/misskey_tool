#include <iostream>


#include <nlohmann/json.hpp>
#include <cpprest/details/basic_types.h>

/*
std::optional<std::vector<std::string>> getFederation(const utility::string_t&  URL){
    std::string data = fetchFederation(URL);
    if (data[0] == '[' | data[0] == '{'){

    } else {
        return std::nullopt;
    }
}*/
#include "misskey.hpp"

int main() {
    //getFederation();
    std::cout <<  misskey::fetchFederation(L"https://oflosky.com") << std::endl ;
    return 0;
}

