#include "core/TaskRepository.h"

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
struct TestCase {
    const char* name;
    std::function<void()> fn;
};

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

void testAddAndTrim() {
    todo::TaskRepository repo;
    auto& task = repo.add("   Learn CMake   ");
    require(task.description() == "Learn CMake", "description must be trimmed");
    require(task.id() == 1, "first id must be 1");
}

void testRejectBlank() {
    todo::TaskRepository repo;
    bool thrown = false;
    try { repo.add("   \t  "); } catch (const std::invalid_argument&) { thrown = true; }
    require(thrown, "blank task must be rejected");
}

void testToggle() {
    todo::TaskRepository repo;
    const auto id = repo.add("Task").id();
    require(repo.toggle(id), "existing task should toggle");
    require(repo.find(id)->completed(), "task should be completed");
    require(repo.toggle(id), "second toggle should work");
    require(!repo.find(id)->completed(), "task should be active again");
}

void testEdit() {
    todo::TaskRepository repo;
    const auto id = repo.add("Old").id();
    require(repo.edit(id, "New", todo::Priority::High, std::string("2026-09-10")), "edit should succeed");
    const auto* task = repo.find(id);
    require(task && task->description() == "New", "description should update");
    require(task->priority() == todo::Priority::High, "priority should update");
    require(task->dueDate().value_or("") == "2026-09-10", "due date should update");
}

void testDelete() {
    todo::TaskRepository repo;
    const auto first = repo.add("First").id();
    repo.add("Second");
    require(repo.remove(first), "remove should succeed");
    require(repo.all().size() == 1, "one task should remain");
    require(repo.find(first) == nullptr, "removed id must not be found");
}

void testSearchAndStatusFilter() {
    todo::TaskRepository repo;
    const auto doneId = repo.add("Write documentation").id();
    repo.add("Write unit tests");
    repo.add("Deploy tracker");
    repo.toggle(doneId);
    const auto writeMatches = repo.search("write");
    require(writeMatches.size() == 2, "case-insensitive search should return two matches");
    const auto active = repo.search("", false);
    require(active.size() == 2, "active filter should hide completed task");
    const auto completed = repo.search("", true);
    require(completed.size() == 1, "completed filter should return one task");
}

void testOverdue() {
    todo::Task active(1, "Old task", false, todo::Priority::Normal, std::string("2026-09-01"));
    require(active.isOverdue("2026-09-09"), "active task before today should be overdue");
    active.setCompleted(true);
    require(!active.isOverdue("2026-09-09"), "completed task should not be overdue");
}

void testReplaceAllContinuesIds() {
    todo::TaskRepository repo;
    std::vector<todo::Task> imported;
    imported.emplace_back(10, "Imported");
    repo.replaceAll(std::move(imported));
    require(repo.add("Next").id() == 11, "new ids must continue after imported maximum");
}

const std::vector<TestCase>& tests() {
    static const std::vector<TestCase> cases = {
        {"add_and_trim", testAddAndTrim},
        {"reject_blank", testRejectBlank},
        {"toggle", testToggle},
        {"edit", testEdit},
        {"delete", testDelete},
        {"search_and_status_filter", testSearchAndStatusFilter},
        {"overdue", testOverdue},
        {"replace_all_continues_ids", testReplaceAllContinuesIds},
    };
    return cases;
}

int runOne(const TestCase& test) {
    try {
        test.fn();
        std::cout << "[PASS] " << test.name << '\n';
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "[FAIL] " << test.name << ": " << ex.what() << '\n';
        return 1;
    }
}
}

int main(int argc, char** argv) {
    if (argc == 2) {
        const std::string requested = argv[1];
        for (const auto& test : tests()) {
            if (requested == test.name) return runOne(test);
        }
        std::cerr << "Unknown test: " << requested << '\n';
        return 2;
    }

    int failures = 0;
    for (const auto& test : tests()) failures += runOne(test);
    std::cout << "Tests: " << tests().size() << ", failures: " << failures << '\n';
    return failures == 0 ? 0 : 1;
}
