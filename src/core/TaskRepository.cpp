#include "core/TaskRepository.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>

namespace todo {

Task& TaskRepository::add(std::string description,
                          Priority priority,
                          std::optional<std::string> dueDate) {
    description = trim(std::move(description));
    if (description.empty()) {
        throw std::invalid_argument("Task description must not be blank");
    }
    tasks_.emplace_back(nextId(), std::move(description), false, priority, std::move(dueDate));
    return tasks_.back();
}

bool TaskRepository::edit(std::uint64_t id,
                          std::string description,
                          Priority priority,
                          std::optional<std::string> dueDate) {
    Task* task = find(id);
    if (!task) return false;
    description = trim(std::move(description));
    if (description.empty()) {
        throw std::invalid_argument("Task description must not be blank");
    }
    task->setDescription(std::move(description));
    task->setPriority(priority);
    task->setDueDate(std::move(dueDate));
    return true;
}

bool TaskRepository::remove(std::uint64_t id) {
    const auto oldSize = tasks_.size();
    tasks_.erase(std::remove_if(tasks_.begin(), tasks_.end(),
                                [id](const Task& task) { return task.id() == id; }),
                 tasks_.end());
    return tasks_.size() != oldSize;
}

bool TaskRepository::toggle(std::uint64_t id) {
    Task* task = find(id);
    if (!task) return false;
    task->toggleCompleted();
    return true;
}

bool TaskRepository::setImagePath(std::uint64_t id, std::string path) {
    Task* task = find(id);
    if (!task) return false;
    task->setImagePath(std::move(path));
    return true;
}

Task* TaskRepository::find(std::uint64_t id) {
    auto it = std::find_if(tasks_.begin(), tasks_.end(),
                           [id](const Task& task) { return task.id() == id; });
    return it == tasks_.end() ? nullptr : &(*it);
}

const Task* TaskRepository::find(std::uint64_t id) const {
    auto it = std::find_if(tasks_.begin(), tasks_.end(),
                           [id](const Task& task) { return task.id() == id; });
    return it == tasks_.end() ? nullptr : &(*it);
}

std::vector<Task> TaskRepository::search(const std::string& query,
                                         std::optional<bool> completedFilter) const {
    const std::string normalizedQuery = lower(trim(query));
    std::vector<Task> result;
    for (const Task& task : tasks_) {
        if (completedFilter.has_value() && task.completed() != *completedFilter) {
            continue;
        }
        if (!normalizedQuery.empty()) {
            const std::string haystack = lower(task.description());
            if (haystack.find(normalizedQuery) == std::string::npos) {
                continue;
            }
        }
        result.push_back(task);
    }
    return result;
}

const std::vector<Task>& TaskRepository::all() const noexcept { return tasks_; }

void TaskRepository::replaceAll(std::vector<Task> tasks) {
    tasks_ = std::move(tasks);
    std::uint64_t maxId = 0;
    for (const Task& task : tasks_) {
        maxId = std::max(maxId, task.id());
    }
    nextId_ = maxId + 1;
}

void TaskRepository::clear() noexcept {
    tasks_.clear();
    nextId_ = 1;
}

std::string TaskRepository::trim(std::string value) {
    const auto isNotSpace = [](unsigned char c) { return !std::isspace(c); };
    auto first = std::find_if(value.begin(), value.end(), isNotSpace);
    auto last = std::find_if(value.rbegin(), value.rend(), isNotSpace).base();
    if (first >= last) return {};
    return std::string(first, last);
}

std::string TaskRepository::lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

std::uint64_t TaskRepository::nextId() noexcept { return nextId_++; }

} // namespace todo
