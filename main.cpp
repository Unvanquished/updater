#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <QFile>
#include <QFontDatabase>
#include <QQmlContext>

#include "iconsimageprovider.h"
#include "iconthemeimageprovider.h"

#include "qmldownloader.h"
#include "settings.h"

namespace {

QFile logFile;

void LogMessageHandler(QtMsgType, const QMessageLogContext&, const QString& msg) {
    logFile.write(msg.toUtf8());
    logFile.write("\n");
    logFile.flush();
}

} // namespace

int main(int argc, char *argv[])
{
    // The two following variables set the config file path
    // to: ~/.config/unvanquished/updater.conf
    QCoreApplication::setApplicationName("updater");
    QCoreApplication::setOrganizationName("unvanquished");
    QCoreApplication::setOrganizationDomain("unvanquished.net");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    QCommandLineOption logFileNameOption("logfile");
    logFileNameOption.setValueName("filename");
    QCommandLineParser optionParser;
    optionParser.addOption(logFileNameOption);
    optionParser.process(app);
    QString logFilename = optionParser.value(logFileNameOption);
    if (!logFilename.isEmpty()) {
        logFile.setFileName(logFilename);
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qInstallMessageHandler(&LogMessageHandler);
        } else {
            qDebug() << "Failed to open log file for writing";
        }
    }

    qDebug() << "Git version:" << GIT_VERSION;

    app.setWindowIcon(QIcon(":resources/updater.png"));
    int fontId = QFontDatabase::addApplicationFont(":resources/Roboto-Regular.ttf");
    if (fontId != -1) {
        QFont font("Roboto-Regular");
        font.setPointSize(10);
        app.setFont(font);
    }
    QQmlApplicationEngine engine;
    engine.addImportPath(QLatin1String("qrc:/"));
    engine.addImageProvider(QLatin1String("fluidicons"), new IconsImageProvider());
    engine.addImageProvider(QLatin1String("fluidicontheme"), new IconThemeImageProvider());
    auto* context = engine.rootContext();
    Settings settings;
    QmlDownloader downloader;
    context->setContextProperty("updaterSettings", &settings);
    context->setContextProperty("downloader", &downloader);
    qmlRegisterType<QmlDownloader>("QmlDownloader", 1, 0, "QmlDownloader");

    engine.load(QUrl(QLatin1String("qrc:/splash.qml")));
    return app.exec();
}
