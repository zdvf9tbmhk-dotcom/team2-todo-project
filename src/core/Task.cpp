#include "core/Task.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>

namespace todo {

Task::Task(std::uint64_t id,
           std::string description,
           bool completed,
           Priority priority,
           std::optional<std::string> dueDate,
           std::string imagePath)
    : id_(id),
      description_(std::move(description)),
      completed_(completed),
      priority_(priority),
      dueDate_(std::move(dueDate)),
      imagePath_(std::move(imagePath)) {
    if (description_.empty()) {
        throw std::invalid_argument("Task description must not be empty");
    }
}

std::uint64_t Task::id() const noexcept { return id_; }
const std::string& Task::description() const noexcept { return description_; }
bool Task::completed() const noexcept { return completed_; }
Priority Task::priority() const noexcept { return priority_; }
const std::optional<std::string>& Task::dueDate() const noexcept { return dueDate_; }
const std::string& Task::imagePath() const noexcept { return imagePath_; }

void Task::setDescription(std::string description) {
    if (description.empty()) {
        throw std::invalid_argument("Task description must not be empty");
    }
    description_ = std::move(description);
}

void Task::setCompleted(bool completed) noexcept { completed_ = completed; }
void Task::toggleCompleted() noexcept { completed_ = !completed_; }
void Task::setPriority(Priority priority) noexcept { priority_ = priority; }
void Task::setDueDate(std::optional<std::string> dueDate) { dueDate_ = std::move(dueDate); }
void Task::setImagePath(std::string path) { imagePath_ = std::move(path); }

bool Task::isOverdue(const std::string& todayIsoDate) const {
    return !completed_ && dueDate_.has_value() && !dueDate_->empty() && *dueDate_ < todayIsoDate;
}

std::string toString(Priority priority) {
    switch (priority) {
        case Priority::Low: return "low";
        case Priority::Normal: return "normal";
        case Priority::High: return "high";
    }
    return "normal";
}

Priority priorityFromString(const std::string& value) {
    std::string normalized = value;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (normalized == "low") return Priority::Low;
    if (normalized == "high") return Priority::High;
    return Priority::Normal;
}

} // namespace todo
