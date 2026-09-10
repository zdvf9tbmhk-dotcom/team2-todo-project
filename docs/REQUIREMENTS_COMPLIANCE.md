# Матрица выполнения требований задания 1

Статусы разделены на **выполнено и проверено**, **выполнено как артефакт/конфигурация** и **требует внешнего доступа/согласования**. Внешние действия не выдаются за завершённые.

## Цели группы

| Требование | Реализация / доказательство | Статус |
|---|---|---|
| Анализ и ревизия кода, журнал | `docs/AUDIT_LOG.md`, 16 проблем/решений, issue #4 | Выполнено |
| Описание проекта CMake | `CMakeLists.txt`: project description, `todo_core`, `todo_app`, `todo_tests`, `check` | Выполнено |
| Собрать проект | core+tests собраны локально; полный Qt 6 build прошёл GitHub Actions run `34454695339` | Выполнено и проверено |
| Написать тесты | 8 отдельных CTest-тестов T01-T08 | Выполнено и проверено |
| Реализовать 5 фичей | edit, delete, priority, due date/overdue, search/status filter | Выполнено; Qt-компиляция подтверждена CI |
| Проверять тестами во время сборки | CMake target `check ALL` запускает CTest внутри `cmake --build`; CI step SUCCESS | Выполнено и проверено |
| Сборочная инфраструктура | CMake/Ninja + GitHub Actions + Jenkinsfile + Dockerfile | GitHub CI работает; кафедральный Jenkins подготовлен |
| Трекер с исполнителями, сроками, %, Kanban | OpenProject выбран и подготовлен; реальные GitHub Issues #1-#17 содержат ответственных, сроки и прогресс | Полный OpenProject/Kanban требует внешнего сервера |
| Git-репозиторий | `https://github.com/zdvf9tbmhk-dotcom/team2-todo-project`, feature branch, CI, Issues | Выполнено |

## Менеджер проекта — Ткачев

Подобран OpenProject; зафиксированы ресурсы/сроки; выбраны Ubuntu 24.04, CMake/Ninja/Qt6, Jenkins/GitHub Actions, Qt Creator и правила `.clang-format`; подготовлены ТЗ, план, тесты и 5 фич; реально заведены GitHub Issues #1-#17; подготовлены контроль качества и сценарий сдачи. Пункты, где формулировка требует подтверждения преподавателя или ресурса кафедры, вынесены в открытые issues #13, #14, #15 и не помечены фиктивно как завершённые.

## DevOps — Михалев

Подготовлены OpenProject/Gitea/Jenkins Docker Compose, Dockerfile, Jenkinsfile и Ubuntu bootstrap. Процесс сборки CMake+CTest работает. Полная Qt 6 сборка в GitHub Actions run `34454695339` завершилась SUCCESS; issue #11 закрыта. Размещение сервисов именно на кафедральной VM требует адреса/учётных данных и отслеживается issue #14; аккаунты внешнего OpenProject — issue #15.

## Разработчик — Поляков Даниил Андреевич

Создан Git workflow; исправлены проблемы из 16-пунктного audit; реализованы CMake, 8 тестов и 5 фич; проект подготовлен к сдаче, сценарий есть в `DELIVERY_SCENARIO.md`; архитектура core/UI описана отдельно. Локальные 8/8 CTest и полная Qt CI-сборка успешно пройдены.

## Реальные GitHub-доказательства

- Repository: `https://github.com/zdvf9tbmhk-dotcom/team2-todo-project`.
- Branch: `project/complete-implementation`.
- Implementation commit: `1e7a551172e6fc55f9a1ee4b87a90d7f48bce9aa`.
- Documentation commit: `0bd1dcba8e353a57def8c6d68e9fece5ce1edb71`.
- Full Qt CI run: `https://github.com/zdvf9tbmhk-dotcom/team2-todo-project/actions/runs/34454695339` — SUCCESS.
- Backlog: GitHub Issues #1-#17; законченные задачи закрыты, реальные внешние блокеры открыты.

## Внешние пункты, которые нельзя честно завершить без третьей стороны

1. Согласование преподавателем — issue #13.
2. Развёртывание OpenProject/Gitea/Jenkins именно на ресурсах кафедры — issue #14.
3. Создание учётных записей Михалева и Ткачева во внешнем OpenProject — issue #15.
4. Интерактивный GUI smoke-test в графическом сеансе — issue #16 (полная Qt-компиляция уже подтверждена CI).
5. Фактическая сдача преподавателю — issue #17.
