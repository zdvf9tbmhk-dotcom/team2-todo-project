#pragma once

#include "core/Task.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace todo {

class TaskRepository {
public:
    TaskRepository() = default;

    Task& add(std::string description,
              Priority priority = Priority::Normal,
              std::optional<std::string> dueDate = std::nullopt);

    bool edit(std::uint64_t id,
              std::string description,
              Priority priority,
              std::optional<std::string> dueDate);
    bool remove(std::uint64_t id);
    bool toggle(std::uint64_t id);
    bool setImagePath(std::uint64_t id, std::string path);

    Task* find(std::uint64_t id);
    const Task* find(std::uint64_t id) const;

    std::vector<Task> search(const std::string& query,
                             std::optional<bool> completedFilter = std::nullopt) const;

    const std::vector<Task>& all() const noexcept;
    void replaceAll(std::vector<Task> tasks);
    void clear() noexcept;

private:
    static std::string trim(std::string value);
    static std::string lower(std::string value);
    std::uint64_t nextId() noexcept;

    std::vector<Task> tasks_;
    std::uint64_t nextId_ = 1;
};

} // namespace todo
