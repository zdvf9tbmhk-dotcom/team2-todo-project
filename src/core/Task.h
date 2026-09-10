#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace todo {

enum class Priority {
    Low = 0,
    Normal = 1,
    High = 2
};

class Task {
public:
    Task(std::uint64_t id,
         std::string description,
         bool completed = false,
         Priority priority = Priority::Normal,
         std::optional<std::string> dueDate = std::nullopt,
         std::string imagePath = {});

    std::uint64_t id() const noexcept;
    const std::string& description() const noexcept;
    bool completed() const noexcept;
    Priority priority() const noexcept;
    const std::optional<std::string>& dueDate() const noexcept;
    const std::string& imagePath() const noexcept;

    void setDescription(std::string description);
    void setCompleted(bool completed) noexcept;
    void toggleCompleted() noexcept;
    void setPriority(Priority priority) noexcept;
    void setDueDate(std::optional<std::string> dueDate);
    void setImagePath(std::string path);

    bool isOverdue(const std::string& todayIsoDate) const;

private:
    std::uint64_t id_;
    std::string description_;
    bool completed_;
    Priority priority_;
    std::optional<std::string> dueDate_;
    std::string imagePath_;
};

std::string toString(Priority priority);
Priority priorityFromString(const std::string& value);

} // namespace todo
