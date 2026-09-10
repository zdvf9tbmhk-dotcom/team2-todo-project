#pragma once

#include "core/TaskRepository.h"

#include <QMainWindow>
#include <cstdint>

class QCheckBox;
class QComboBox;
class QDateEdit;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;

class ToDoListApp final : public QMainWindow {
    Q_OBJECT

public:
    explicit ToDoListApp(QWidget* parent = nullptr);

private slots:
    void addTask();
    void editTask();
    void deleteTask();
    void toggleTaskComplete(QListWidgetItem* item);
    void addImageToTask();
    void saveTasksAs();
    void loadTasksFrom();
    void applyFilter();
    void updateSelectionPreview();

private:
    void setupUi();
    void refreshTaskList();
    void populateEditor(const todo::Task* task);
    void clearEditor();
    void saveCache();
    void loadCache();
    bool saveJson(const QString& filePath, bool showMessages);
    bool loadJson(const QString& filePath, bool showMessages);
    std::uint64_t selectedTaskId() const;
    todo::Priority selectedPriority() const;
    QString cacheFilePath() const;

    todo::TaskRepository repository_;

    QLineEdit* taskInput_ = nullptr;
    QComboBox* priorityCombo_ = nullptr;
    QCheckBox* dueDateEnabled_ = nullptr;
    QDateEdit* dueDateEdit_ = nullptr;
    QPushButton* addButton_ = nullptr;
    QPushButton* editButton_ = nullptr;
    QPushButton* deleteButton_ = nullptr;
    QListWidget* taskList_ = nullptr;
    QLineEdit* searchInput_ = nullptr;
    QComboBox* statusFilter_ = nullptr;
    QPushButton* saveButton_ = nullptr;
    QPushButton* loadButton_ = nullptr;
    QPushButton* addImageButton_ = nullptr;
    QLabel* imageLabel_ = nullptr;
    QLabel* summaryLabel_ = nullptr;
};
