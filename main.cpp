#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "iconsimageprovider.h"
#include "iconthemeimageprovider.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.addImportPath(QLatin1String("qrc:/"));
    engine.addImageProvider(QLatin1String("fluidicons"), new IconsImageProvider());
    engine.addImageProvider(QLatin1String("fluidicontheme"), new IconThemeImageProvider());
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
