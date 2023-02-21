#include <iostream>
#include <cpprest/http_client.h>

std::string getFederation(const utility::string_t&  URL){
    web::http::client::http_client client(URL + L"/api/federation/instances");
    auto response = client.request(web::http::methods::POST, L"", L"{}" , L"application/json").get();
    auto body = response.extract_utf8string().get();
    return body ;
};

int main() {
    std::cout << getFederation(L"https://oflosky.com") << std::endl;

    return 0;
}

