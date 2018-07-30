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
#include "system.h"

namespace {

QFile logFile;

void LogMessageHandler(QtMsgType, const QMessageLogContext&, const QString& msg) {
    logFile.write(msg.toUtf8());
    logFile.write("\n");
    logFile.flush();
}

void LogSettings() {
    std::unique_ptr<QSettings> settings(Sys::makePersistentSettings(nullptr));
    QStringList storedKeys = settings->allKeys();
    qDebug() << storedKeys.size() << "stored settings:";
    for (const auto& key : storedKeys) {
        qDebug() << " -" << key << "=" << settings->value(key).toString();
    }
}

} // namespace

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("Unvanquished Updater");
    QCoreApplication::setOrganizationName("Unvanquished Development");
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
    LogSettings();
    try {
        qDebug() << "Testing exception handling...";
        throw 1;
    } catch(int) {
        qDebug() << "Exception handling works";
    }

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
