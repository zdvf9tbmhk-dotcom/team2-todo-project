# Team 2 To-Do List (C++ / Qt / CMake)

[![CMake CI](https://github.com/zdvf9tbmhk-dotcom/team2-todo-project/actions/workflows/ci.yml/badge.svg)](https://github.com/zdvf9tbmhk-dotcom/team2-todo-project/actions/workflows/ci.yml)

**Репозиторий:** https://github.com/zdvf9tbmhk-dotcom/team2-todo-project

Учебный проект команды 2 группы **ККСО-08-22**. За основу взят небольшой Qt/C++ проект `FujiwaraChoki/todo-list-cpp`, указанный в задании как пример. Проект переработан: добавлен CMake, выделена тестируемая бизнес-логика, исправлены ошибки исходной реализации, добавлены тесты, пять функциональных улучшений и CI.

## Команда

- **Ткачев** - менеджер проекта.
- **Михалев** - инженер DevOps.
- **Поляков Даниил Андреевич** - разработчик.

## Реализованные 5 фичей

1. Редактирование существующей задачи.
2. Удаление задачи с подтверждением.
3. Приоритеты Low / Normal / High.
4. Срок выполнения и признак просрочки.
5. Поиск по описанию и фильтр по статусу (все / активные / выполненные).

Сохранены исходные полезные возможности: отметка выполнения, изображение у задачи, автоматический JSON-кэш, ручной импорт/экспорт JSON.

## Быстрая сборка (Ubuntu 24.04)

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake ninja-build qt6-base-dev
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
./build/todo_app
```

Тесты привязаны к обычной сборке через CMake target `check ALL`: при `cmake --build build` CTest запускается автоматически.

## Проверка ядра без Qt

Полезно на машине, где Qt ещё не установлен:

```bash
cmake -S . -B build-core -DTODO_BUILD_GUI=OFF
cmake --build build-core
ctest --test-dir build-core --output-on-failure
```

## Структура

- `src/core` - независимая от Qt модель задач и репозиторий.
- `src/ui` - графический интерфейс Qt Widgets.
- `tests` - 8 автоматических тестов ядра.
- `docs` - ТЗ, журнал ревизии, тест-план, план проекта, сценарий сдачи.
- `.github/workflows/ci.yml` - CI GitHub Actions.
- `Jenkinsfile` - альтернативный pipeline для инфраструктуры кафедры.
- `infra/docker-compose.yml` - OpenProject + Gitea + Jenkins.

## Среда разработки

Рекомендуется Ubuntu 24.04 LTS, GCC 13+ или Clang, Qt 6, CMake 3.21+, Qt Creator. Форматирование - `.clang-format` в репозитории.

## Происхождение

Исходный пример: `https://github.com/FujiwaraChoki/todo-list-cpp` (MIT). В учебной версии архитектура и код существенно переработаны командой 2.

## Полная сверка с заданием

- `docs/REQUIREMENTS_COMPLIANCE.md` — построчная матрица выполнения целей группы и обязанностей всех ролей.
- `docs/APPROVAL_PROTOCOL.md` — готовый пакет для тех пунктов, где по условию требуется реальное согласование преподавателя.
- `docs/BUILD_VERIFICATION.txt` — фактический протокол локальной CMake/CTest-проверки.

Важно: проект не выдаёт внешние действия за выполненные. Развёртывание на сервере кафедры, создание внешних аккаунтов и согласование преподавателем требуют соответствующего доступа/ответа и отмечены отдельно.
