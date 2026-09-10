#include "ui/ToDoListApp.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QStandardPaths>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

#include <optional>
#include <stdexcept>
#include <utility>
#include <string>
#include <vector>

namespace {
QString priorityText(todo::Priority priority) {
    switch (priority) {
        case todo::Priority::Low: return QStringLiteral("LOW");
        case todo::Priority::Normal: return QStringLiteral("NORMAL");
        case todo::Priority::High: return QStringLiteral("HIGH");
    }
    return QStringLiteral("NORMAL");
}

std::optional<std::string> qDateToOptional(const QCheckBox* enabled, const QDateEdit* edit) {
    if (!enabled->isChecked()) return std::nullopt;
    return edit->date().toString(Qt::ISODate).toStdString();
}
}

ToDoListApp::ToDoListApp(QWidget* parent) : QMainWindow(parent) {
    setupUi();
    loadCache();
    refreshTaskList();
}

void ToDoListApp::setupUi() {
    setWindowTitle(tr("Team 2 - To-Do List"));
    resize(820, 620);

    auto* central = new QWidget(this);
    auto* rootLayout = new QVBoxLayout(central);

    auto* editorLayout = new QFormLayout;
    taskInput_ = new QLineEdit(central);
    taskInput_->setPlaceholderText(tr("Описание задачи"));

    priorityCombo_ = new QComboBox(central);
    priorityCombo_->addItem(tr("Низкий"), static_cast<int>(todo::Priority::Low));
    priorityCombo_->addItem(tr("Обычный"), static_cast<int>(todo::Priority::Normal));
    priorityCombo_->addItem(tr("Высокий"), static_cast<int>(todo::Priority::High));
    priorityCombo_->setCurrentIndex(1);

    auto* deadlineRow = new QWidget(central);
    auto* deadlineLayout = new QHBoxLayout(deadlineRow);
    deadlineLayout->setContentsMargins(0, 0, 0, 0);
    dueDateEnabled_ = new QCheckBox(tr("Установить срок"), deadlineRow);
    dueDateEdit_ = new QDateEdit(QDate::currentDate().addDays(1), deadlineRow);
    dueDateEdit_->setCalendarPopup(true);
    dueDateEdit_->setDisplayFormat(QStringLiteral("dd.MM.yyyy"));
    dueDateEdit_->setEnabled(false);
    deadlineLayout->addWidget(dueDateEnabled_);
    deadlineLayout->addWidget(dueDateEdit_);
    deadlineLayout->addStretch();

    editorLayout->addRow(tr("Задача:"), taskInput_);
    editorLayout->addRow(tr("Приоритет:"), priorityCombo_);
    editorLayout->addRow(tr("Срок:"), deadlineRow);
    rootLayout->addLayout(editorLayout);

    auto* actionLayout = new QHBoxLayout;
    addButton_ = new QPushButton(tr("Добавить"), central);
    editButton_ = new QPushButton(tr("Изменить"), central);
    deleteButton_ = new QPushButton(tr("Удалить"), central);
    addImageButton_ = new QPushButton(tr("Прикрепить изображение"), central);
    actionLayout->addWidget(addButton_);
    actionLayout->addWidget(editButton_);
    actionLayout->addWidget(deleteButton_);
    actionLayout->addWidget(addImageButton_);
    rootLayout->addLayout(actionLayout);

    auto* filterLayout = new QHBoxLayout;
    searchInput_ = new QLineEdit(central);
    searchInput_->setPlaceholderText(tr("Поиск по описанию..."));
    statusFilter_ = new QComboBox(central);
    statusFilter_->addItem(tr("Все"));
    statusFilter_->addItem(tr("Активные"));
    statusFilter_->addItem(tr("Выполненные"));
    filterLayout->addWidget(searchInput_, 1);
    filterLayout->addWidget(statusFilter_);
    rootLayout->addLayout(filterLayout);

    taskList_ = new QListWidget(central);
    taskList_->setAlternatingRowColors(true);
    rootLayout->addWidget(taskList_, 1);

    summaryLabel_ = new QLabel(central);
    imageLabel_ = new QLabel(tr("Изображение не выбрано"), central);
    imageLabel_->setMinimumHeight(120);
    imageLabel_->setAlignment(Qt::AlignCenter);
    imageLabel_->setStyleSheet(QStringLiteral("QLabel { border: 1px solid #999; padding: 6px; }"));
    rootLayout->addWidget(summaryLabel_);
    rootLayout->addWidget(imageLabel_);

    auto* fileLayout = new QHBoxLayout;
    saveButton_ = new QPushButton(tr("Сохранить JSON..."), central);
    loadButton_ = new QPushButton(tr("Загрузить JSON..."), central);
    fileLayout->addStretch();
    fileLayout->addWidget(saveButton_);
    fileLayout->addWidget(loadButton_);
    rootLayout->addLayout(fileLayout);

    setCentralWidget(central);

    connect(dueDateEnabled_, &QCheckBox::toggled, dueDateEdit_, &QWidget::setEnabled);
    connect(addButton_, &QPushButton::clicked, this, &ToDoListApp::addTask);
    connect(editButton_, &QPushButton::clicked, this, &ToDoListApp::editTask);
    connect(deleteButton_, &QPushButton::clicked, this, &ToDoListApp::deleteTask);
    connect(taskList_, &QListWidget::itemDoubleClicked, this, &ToDoListApp::toggleTaskComplete);
    connect(taskList_, &QListWidget::itemSelectionChanged, this, &ToDoListApp::updateSelectionPreview);
    connect(addImageButton_, &QPushButton::clicked, this, &ToDoListApp::addImageToTask);
    connect(saveButton_, &QPushButton::clicked, this, &ToDoListApp::saveTasksAs);
    connect(loadButton_, &QPushButton::clicked, this, &ToDoListApp::loadTasksFrom);
    connect(searchInput_, &QLineEdit::textChanged, this, &ToDoListApp::applyFilter);
    connect(statusFilter_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ToDoListApp::applyFilter);
    connect(taskInput_, &QLineEdit::returnPressed, this, &ToDoListApp::addTask);
}

void ToDoListApp::addTask() {
    try {
        repository_.add(taskInput_->text().toStdString(), selectedPriority(),
                        qDateToOptional(dueDateEnabled_, dueDateEdit_));
        clearEditor();
        saveCache();
        refreshTaskList();
    } catch (const std::invalid_argument&) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Введите непустое описание задачи."));
    }
}

void ToDoListApp::editTask() {
    const std::uint64_t id = selectedTaskId();
    if (id == 0) {
        QMessageBox::information(this, tr("Редактирование"), tr("Сначала выберите задачу."));
        return;
    }
    try {
        repository_.edit(id, taskInput_->text().toStdString(), selectedPriority(),
                         qDateToOptional(dueDateEnabled_, dueDateEdit_));
        saveCache();
        refreshTaskList();
    } catch (const std::invalid_argument&) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Описание задачи не может быть пустым."));
    }
}

void ToDoListApp::deleteTask() {
    const std::uint64_t id = selectedTaskId();
    if (id == 0) {
        QMessageBox::information(this, tr("Удаление"), tr("Сначала выберите задачу."));
        return;
    }
    if (QMessageBox::question(this, tr("Удаление"), tr("Удалить выбранную задачу?")) != QMessageBox::Yes) {
        return;
    }
    repository_.remove(id);
    clearEditor();
    saveCache();
    refreshTaskList();
}

void ToDoListApp::toggleTaskComplete(QListWidgetItem* item) {
    if (!item) return;
    const auto id = item->data(Qt::UserRole).toULongLong();
    repository_.toggle(static_cast<std::uint64_t>(id));
    saveCache();
    refreshTaskList();
}

void ToDoListApp::addImageToTask() {
    const std::uint64_t id = selectedTaskId();
    if (id == 0) {
        QMessageBox::information(this, tr("Изображение"), tr("Сначала выберите задачу."));
        return;
    }
    const QString filePath = QFileDialog::getOpenFileName(
        this, tr("Выберите изображение"), QString(), tr("Изображения (*.png *.jpg *.jpeg *.bmp)"));
    if (filePath.isEmpty()) return;
    repository_.setImagePath(id, filePath.toStdString());
    saveCache();
    updateSelectionPreview();
}

void ToDoListApp::saveTasksAs() {
    const QString path = QFileDialog::getSaveFileName(
        this, tr("Сохранить задачи"), QStringLiteral("tasks.json"), tr("JSON (*.json)"));
    if (!path.isEmpty()) saveJson(path, true);
}

void ToDoListApp::loadTasksFrom() {
    const QString path = QFileDialog::getOpenFileName(
        this, tr("Загрузить задачи"), QString(), tr("JSON (*.json)"));
    if (!path.isEmpty() && loadJson(path, true)) {
        saveCache();
        refreshTaskList();
    }
}

void ToDoListApp::applyFilter() { refreshTaskList(); }

void ToDoListApp::updateSelectionPreview() {
    const std::uint64_t id = selectedTaskId();
    const todo::Task* task = repository_.find(id);
    populateEditor(task);

    if (!task || task->imagePath().empty()) {
        imageLabel_->setPixmap(QPixmap());
        imageLabel_->setText(tr("Изображение не выбрано"));
        return;
    }
    QPixmap pixmap(QString::fromStdString(task->imagePath()));
    if (pixmap.isNull()) {
        imageLabel_->setPixmap(QPixmap());
        imageLabel_->setText(tr("Файл изображения недоступен"));
        return;
    }
    imageLabel_->setText(QString());
    imageLabel_->setPixmap(pixmap.scaled(360, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ToDoListApp::refreshTaskList() {
    const std::uint64_t previousId = selectedTaskId();
    taskList_->clear();

    std::optional<bool> status;
    if (statusFilter_->currentIndex() == 1) status = false;
    if (statusFilter_->currentIndex() == 2) status = true;

    const auto filtered = repository_.search(searchInput_->text().toStdString(), status);
    const std::string today = QDate::currentDate().toString(Qt::ISODate).toStdString();

    int completed = 0;
    for (const auto& task : repository_.all()) {
        if (task.completed()) ++completed;
    }

    for (const auto& task : filtered) {
        QString text = QStringLiteral("[%1] %2").arg(priorityText(task.priority()),
                                                     QString::fromStdString(task.description()));
        if (task.dueDate().has_value()) {
            text += tr(" | срок: %1").arg(QString::fromStdString(*task.dueDate()));
        }
        if (task.isOverdue(today)) {
            text += tr(" | ПРОСРОЧЕНО");
        }

        auto* item = new QListWidgetItem(text, taskList_);
        item->setData(Qt::UserRole, QVariant::fromValue<qulonglong>(task.id()));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(task.completed() ? Qt::Checked : Qt::Unchecked);
        if (task.id() == previousId) item->setSelected(true);
    }

    summaryLabel_->setText(tr("Всего: %1 | выполнено: %2 | показано: %3")
                               .arg(static_cast<qulonglong>(repository_.all().size()))
                               .arg(completed)
                               .arg(static_cast<qulonglong>(filtered.size())));
}

void ToDoListApp::populateEditor(const todo::Task* task) {
    if (!task) return;
    taskInput_->setText(QString::fromStdString(task->description()));
    priorityCombo_->setCurrentIndex(static_cast<int>(task->priority()));
    if (task->dueDate().has_value()) {
        const QDate parsed = QDate::fromString(QString::fromStdString(*task->dueDate()), Qt::ISODate);
        dueDateEnabled_->setChecked(true);
        if (parsed.isValid()) dueDateEdit_->setDate(parsed);
    } else {
        dueDateEnabled_->setChecked(false);
    }
}

void ToDoListApp::clearEditor() {
    taskInput_->clear();
    priorityCombo_->setCurrentIndex(1);
    dueDateEnabled_->setChecked(false);
    dueDateEdit_->setDate(QDate::currentDate().addDays(1));
    imageLabel_->setPixmap(QPixmap());
    imageLabel_->setText(tr("Изображение не выбрано"));
}

bool ToDoListApp::saveJson(const QString& filePath, bool showMessages) {
    QJsonArray array;
    for (const auto& task : repository_.all()) {
        QJsonObject obj;
        obj[QStringLiteral("id")] = QString::number(task.id());
        obj[QStringLiteral("description")] = QString::fromStdString(task.description());
        obj[QStringLiteral("completed")] = task.completed();
        obj[QStringLiteral("priority")] = QString::fromStdString(todo::toString(task.priority()));
        obj[QStringLiteral("dueDate")] = task.dueDate().has_value()
            ? QString::fromStdString(*task.dueDate()) : QString();
        obj[QStringLiteral("imagePath")] = QString::fromStdString(task.imagePath());
        array.append(obj);
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (showMessages) QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось открыть файл для записи."));
        return false;
    }
    file.write(QJsonDocument(array).toJson(QJsonDocument::Indented));
    if (showMessages) QMessageBox::information(this, tr("Сохранение"), tr("Задачи сохранены."));
    return true;
}

bool ToDoListApp::loadJson(const QString& filePath, bool showMessages) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        if (showMessages) QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось открыть файл."));
        return false;
    }

    QJsonParseError error;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError || !document.isArray()) {
        if (showMessages) QMessageBox::warning(this, tr("Ошибка"), tr("Некорректный JSON-файл."));
        return false;
    }

    std::vector<todo::Task> loaded;
    for (const QJsonValue& value : document.array()) {
        if (!value.isObject()) continue;
        const QJsonObject obj = value.toObject();
        const QString description = obj.value(QStringLiteral("description")).toString().trimmed();
        if (description.isEmpty()) continue;
        bool ok = false;
        const auto id = obj.value(QStringLiteral("id")).toString().toULongLong(&ok);
        if (!ok || id == 0) continue;
        const bool completed = obj.value(QStringLiteral("completed")).toBool(false);
        const auto priority = todo::priorityFromString(obj.value(QStringLiteral("priority")).toString().toStdString());
        const QString dueDate = obj.value(QStringLiteral("dueDate")).toString();
        std::optional<std::string> due;
        if (!dueDate.isEmpty() && QDate::fromString(dueDate, Qt::ISODate).isValid()) due = dueDate.toStdString();
        const QString imagePath = obj.value(QStringLiteral("imagePath")).toString();
        loaded.emplace_back(static_cast<std::uint64_t>(id), description.toStdString(), completed,
                            priority, std::move(due), imagePath.toStdString());
    }

    repository_.replaceAll(std::move(loaded));
    if (showMessages) QMessageBox::information(this, tr("Загрузка"), tr("Задачи загружены."));
    return true;
}

void ToDoListApp::saveCache() {
    const QString path = cacheFilePath();
    QDir().mkpath(QFileInfo(path).absolutePath());
    saveJson(path, false);
}

void ToDoListApp::loadCache() {
    const QString path = cacheFilePath();
    if (QFile::exists(path)) loadJson(path, false);
}

std::uint64_t ToDoListApp::selectedTaskId() const {
    const QListWidgetItem* item = taskList_ ? taskList_->currentItem() : nullptr;
    if (!item) return 0;
    return static_cast<std::uint64_t>(item->data(Qt::UserRole).toULongLong());
}

todo::Priority ToDoListApp::selectedPriority() const {
    return static_cast<todo::Priority>(priorityCombo_->currentData().toInt());
}

QString ToDoListApp::cacheFilePath() const {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (dir.isEmpty()) dir = QDir::homePath() + QStringLiteral("/.team2-todo");
    return QDir(dir).filePath(QStringLiteral("cached_tasks.json"));
}
