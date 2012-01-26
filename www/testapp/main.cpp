#include <QApplication>
#include <QtDeclarative>

#include "Languages.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Languages languages;

    QDeclarativeView view;
    view.rootContext()->setContextProperty("languages", &languages);

    view.setSource(QUrl::fromLocalFile("main.qml"));

    view.show();

    return app.exec();
}
