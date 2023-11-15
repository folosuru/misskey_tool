#ifndef MISSKEY_TOOL_SRC_QUEUE_WORK_QUEUE_HPP
#define MISSKEY_TOOL_SRC_QUEUE_WORK_QUEUE_HPP
#include <queue>
#include <string>
#include <unordered_set>
#include <utility>
#include <shared_mutex>
#include <optional>
class work_queue;
class target_domain {
public:
    std::string domain;

    void addQueue(const std::string&);

    target_domain(std::string  domain, work_queue& queue);

private:
    work_queue& queue;
};


class work_queue {
public:
    void add(const std::string&);

    bool isNotFound(const std::string&);

    std::optional<target_domain> get();
private:
    std::queue<std::string> queue;
    std::unordered_set<std::string> found_domain;
    std::shared_mutex found_mutex;
    std::shared_mutex queue_mutex;
};
#endif  // MISSKEY_TOOL_SRC_QUEUE_WORK_QUEUE_HPP
