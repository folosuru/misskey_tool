#include <iostream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include "software_api/api.hpp"
#include <pqxx/pqxx>
#include <thread>
#include "util/util.hpp"
#include "util/sql.hpp"
#include "util/blacklist.hpp"
#include "queue/work_queue.hpp"

int main() {
    pqxx::connection connection = util::sql::createConnection();
    try{
        util::sql::initDB(connection);
    } catch (std::exception const &e){
        std::cerr << "Database setup error: " << e.what() << std::endl;
        return 1;
    }
    std::shared_ptr<work_queue> queue = std::make_shared<work_queue>();
    std::shared_ptr<util::blacklist> blacklist;
    try {
        blacklist = std::make_shared<util::blacklist>(connection);
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    {
        pqxx::work tx{connection};
        for (auto [name] : tx.stream<std::string_view>("SELECT domain from instance_list")) {
            queue->addFoundAndQueue_unsafe(name);
        }
    }
    connection.close();
    std::cout << "added queue:" << queue->getQueueSize() << "\n";

    /* first instance...
     *
     * you can change url to another instance.
     */
    //queue->add("msky.z-n-a-k.net");
    //api::getInstance(queue->get().value(), blacklist)->fetchFederationToQueue();
    std::vector<std::thread> thread_list;
    for (int i = 0; i < 15; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        thread_list.emplace_back([queue,blacklist] {
            std::cout << "start thread..." << std::endl;
            pqxx::connection db = util::sql::createConnection();
            while (true) {
                std::optional<target_domain> target = queue->get();
                //std::cout << "start: " << target.value().domain << std::endl;
                if (!target) {
                    break;
                }
                if (blacklist->isBlacklisted(target.value().domain)){
                    continue;
                }
                std::shared_ptr<api> i;
                try {
                    i = api::getInstance(target.value(), blacklist);
                } catch (std::exception &exception) {
                    std::string topLevelDomain = util::getTopLevelDomain(target.value().domain);
                    if (blacklist->addBlacklistCandidate(topLevelDomain)) {
                        util::sql::addBlacklist(db, topLevelDomain);
                    }
                    continue;
                }
                try {
                    util::sql::writeInstance(db, i);
                } catch (std::exception& e) {
                    std::cout << "exception in get information (or write database): " << i->getDomain() << e.what() << std::endl;
                }
                try {
                    i->fetchFederationToQueue();
                } catch (std::exception& e) {
                    std::cout << "fail to get federation: " << i->getDomain() << e.what() << std::endl;
                }
            }
            db.close();
            std::cout << "thread end" << std::endl;
        });
    }
    std::thread logger([queue,blacklist]{
        int count = 0;
        while (true){
            std::this_thread::sleep_for(std::chrono::minutes(30));
            ++count;
            auto size = queue->getQueueSize();
            std::cout << count << ": queue :" << size << std::endl;
            std::cout << count << ": result:" << queue->getFoundSize() << std::endl;
            if (size == 0) break;
        }
    });
    for (auto& thread : thread_list) {
        thread.join();
    }
    logger.join();
    return 0;
}