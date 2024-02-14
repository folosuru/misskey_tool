#ifndef MISSKEY_TOOL_SRC_QUEUE_WORK_QUEUE_HPP
#define MISSKEY_TOOL_SRC_QUEUE_WORK_QUEUE_HPP
#include <queue>
#include <string>
#include <unordered_set>
#include <utility>
#include <shared_mutex>
#include <optional>
#include <stack>
class work_queue;
class target_domain {
public:
    std::string domain;

    void addQueue(const std::string&) noexcept;

    target_domain(std::string  domain, work_queue& queue);

private:
    work_queue& queue;
};


class work_queue {
public:
    void addFoundAndQueue_unsafe(std::string_view) noexcept;

    void add(const std::string&) noexcept;

    bool isNotFound(const std::string&) const noexcept;

    std::optional<target_domain> get() noexcept;

    size_t getQueueSize() const noexcept;
    size_t getFoundSize() const noexcept;
private:
    std::stack<std::string_view> queue;
    std::unordered_set<std::string> found_domain;
    mutable std::shared_mutex found_mutex;
    mutable std::shared_mutex queue_mutex;
};
#endif  // MISSKEY_TOOL_SRC_QUEUE_WORK_QUEUE_HPP
