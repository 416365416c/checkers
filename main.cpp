#include <QObject>
#include <QApplication>
#include <QDeclarativeView>
#include <QDeclarativeEngine>
#include <qdeclarative.h>
#include "aibrain.h"

int main (int argc, char* argv[])
{
    QApplication app(argc, argv);
    QDeclarativeView view;
    qmlRegisterType<AIBrain>("App", 1, 0, "AIBrain");
    view.setSource(QUrl("qrc:qml/checkers.qml"));
    view.show();
    QObject::connect(view.engine(), SIGNAL(quit()),
            &app, SLOT(quit()));
    return app.exec();
}
