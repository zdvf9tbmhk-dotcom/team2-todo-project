#include "ui/ToDoListApp.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Team2Todo"));
    QApplication::setOrganizationName(QStringLiteral("KKSO-08-22-Team2"));

    ToDoListApp window;
    window.show();
    return app.exec();
}
