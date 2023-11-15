#include <queue>
class target_domain {
public:
    const std::string domain;

    void addQueue(std::string);

    void resolve();
private:
    std::unordered_set<std::string>& resolved_ref;
    work_queue& queue;
};


class work_queue {
public:
    void add(std::string);

    target_domain get();
private:
    std::queue<std::string> queue;
    std::unordered_set<std::string> resolved;
};
