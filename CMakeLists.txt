cmake_minimum_required(VERSION 3.21)
project(Team2Todo VERSION 1.0.0 DESCRIPTION "Учебный менеджер задач команды 2 ККСО-08-22" HOMEPAGE_URL "https://github.com/zdvf9tbmhk-dotcom/team2-todo-project" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(TODO_BUILD_GUI "Build the Qt GUI application" ON)
option(TODO_BUILD_TESTS "Build unit tests" ON)
option(TODO_RUN_TESTS_ON_BUILD "Run tests as part of the normal build" ON)

add_library(todo_core
    src/core/Task.cpp
    src/core/TaskRepository.cpp
)
target_include_directories(todo_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(todo_core PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(todo_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(todo_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(TODO_BUILD_GUI)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

    set(CMAKE_AUTOMOC ON)

    add_executable(todo_app
        src/main.cpp
        src/ui/ToDoListApp.cpp
        src/ui/ToDoListApp.h
    )
    target_link_libraries(todo_app PRIVATE todo_core Qt${QT_VERSION_MAJOR}::Widgets)
    target_include_directories(todo_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

    if(MSVC)
        target_compile_options(todo_app PRIVATE /W4 /permissive-)
    else()
        target_compile_options(todo_app PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

include(CTest)
if(BUILD_TESTING AND TODO_BUILD_TESTS)
    add_executable(todo_tests tests/test_main.cpp)
    target_link_libraries(todo_tests PRIVATE todo_core)
    target_include_directories(todo_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

    if(MSVC)
        target_compile_options(todo_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(todo_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    set(TODO_TEST_CASES
        add_and_trim
        reject_blank
        toggle
        edit
        delete
        search_and_status_filter
        overdue
        replace_all_continues_ids
    )
    foreach(test_case IN LISTS TODO_TEST_CASES)
        add_test(NAME todo_${test_case} COMMAND todo_tests ${test_case})
    endforeach()

    if(TODO_RUN_TESTS_ON_BUILD)
        add_custom_target(check ALL
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            DEPENDS todo_tests
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running CTest as part of the build"
        )
    endif()
endif()
