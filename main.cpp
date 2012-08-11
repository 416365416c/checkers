#include <QObject>
#include <QApplication>
#include <QDeclarativeView>
#include <QDeclarativeEngine>

int main (int argc, char* argv[])
{
    QApplication app(argc, argv);
    QDeclarativeView view;
    view.setSource(QUrl("qrc:qml/checkers.qml"));
    view.show();
    QObject::connect(view.engine(), SIGNAL(quit()),
            &app, SLOT(quit()));
    return app.exec();
}
