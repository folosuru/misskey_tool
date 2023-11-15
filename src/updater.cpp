//
// Created by folosuru on 2023/03/27.
//
#include <iostream>
#include "software_api/api.hpp"

int main() {
    auto q = std::make_shared<work_queue>();
    q->add("oflosky.com");
    api::getInstance(q->get().value(), std::make_shared<util::blacklist>())->fetchFederationToQueue();
    return 0;
}
