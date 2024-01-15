#include "work_queue.hpp"
#include <mutex>
#include <shared_mutex>
#include <utility>


target_domain::target_domain(std::string domain,
                             work_queue& queue) : domain(std::move(domain)), queue(queue) {}


void target_domain::addQueue(const std::string& add_domain) noexcept {
    queue.add(add_domain);
}

void work_queue::addFoundAndQueue_unsafe(std::string_view view) noexcept {
    queue.insert(std::string{view});
    found_domain.insert(std::string{view});
}

void work_queue::add(const std::string& add_domain) noexcept {
    std::shared_lock<std::shared_mutex> lock(found_mutex);
    if (found_domain.find(add_domain) == found_domain.end()) {
        std::lock_guard<std::shared_mutex> lock2(queue_mutex);
        queue.insert(add_domain);
    }
}

std::optional<target_domain> work_queue::get() noexcept {
    std::string url;
    {
        std::lock_guard<std::shared_mutex> lock(queue_mutex);
        if (queue.empty()) return std::nullopt;
        url = *queue.begin();
        queue.erase(url);
    }
    {
        std::lock_guard<std::shared_mutex> lock2(found_mutex);
        found_domain.insert(url);
    }
    return target_domain(target_domain(url, *this));
}

bool work_queue::isNotFound(const std::string& name) const noexcept {
    std::shared_lock<std::shared_mutex> lock(found_mutex);
    if (found_domain.find(name) == found_domain.end()) {
        return true;
    }
    return false;
}

size_t work_queue::getQueueSize() const noexcept{
    std::shared_lock<std::shared_mutex> lock(queue_mutex);
    return queue.size();
}

size_t work_queue::getFoundSize() const noexcept {
    std::shared_lock<std::shared_mutex> lock(found_mutex);
    return found_domain.size();
}
