#include "work_queue.hpp"
#include <shared_mutex>
#include <utility>


target_domain::target_domain(std::string domain,
                             work_queue& queue) : domain(std::move(domain)), queue(queue) {}


void target_domain::addQueue(const std::string& add_domain) {
    queue.add(add_domain);
}

void work_queue::add(const std::string& add_domain) {
    std::shared_lock<std::shared_mutex> lock(found_mutex);
    if (isNotFound(add_domain)) {
        std::lock_guard<std::shared_mutex> lock2(queue_mutex);
        queue.push(add_domain);
    }
}

std::optional<target_domain> work_queue::get() {
    std::lock_guard<std::shared_mutex> lock(queue_mutex);
    auto result = target_domain(queue.front(), *this);
    queue.pop();
    return result;
}

bool work_queue::isNotFound(const std::string& name) {
    std::shared_lock<std::shared_mutex> lock(found_mutex);
    if (found_domain.find(name) == found_domain.end()) {
        return true;
    }
    return false;
}
