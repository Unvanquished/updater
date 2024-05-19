/* Unvanquished Updater
 * Copyright (C) Unvanquished Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDateTime>
#include <QQmlApplicationEngine>
#include <QFile>
#include <QFontDatabase>
#include <QQmlContext>
#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>

#include "iconsimageprovider.h"
#include "iconthemeimageprovider.h"

#include "gamelauncher.h"
#include "qmldownloader.h"
#include "settings.h"
#include "splashcontroller.h"
#include "system.h"

namespace {

QFile logFile;

void LogMessageHandler(QtMsgType, const QMessageLogContext&, const QString& msg) {
    static const QString timeFormat = "yyyy-MM-dd hh:mm:ssZ  ";
    logFile.write(QDateTime::currentDateTimeUtc().toString(timeFormat).toUtf8());
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

struct CommandLineOptions {
    QString logFilename;
    QString ariaLogFilename;
    int splashMilliseconds = 3000;
    RelaunchCommand relaunchCommand = RelaunchCommand::NONE;
    QString updateUpdaterVersion;
    QString connectUrl;
};

static void argParseError(const QString& message)
{
    QMessageBox::critical(nullptr, "Unvanquished Updater", message);
    exit(1);
}

QString getURLFromPositionalOptions(const QCommandLineParser& parser)
{
    QStringList args = parser.positionalArguments();
    if (args.empty()) {
        return "";
    } else if (args.size() > 1) {
        argParseError("Too many command line arguments.");
    }

    QString url = args[0];
    QRegularExpression scheme("^[a-zA-Z][-+.a-zA-Z]*://");
    url.replace(scheme, ""); // Strip scheme

    int passwordEnd = url.indexOf('@') + 1; // relies on -1 => not found
    if (passwordEnd >= url.size()) {
        argParseError("URL argument is empty");
    }

    // Square brackets are shell globbing metacharacters but may be used in IPv6 addresses.
    // Take advantage of a Daemon parsing quirk that it will still be understood without the
    // leading '['.
    if (url[passwordEnd] == '[') {
        url.remove(passwordEnd, 1);
    }

    // If a custom command is used, we have to consider the pessimistic case
    // of a command line like   bash -c "%command%"
    // so metacharacters from any common shells must be forbidden.
    // What we need to accept are domain names and IP addresses.
    // Also throw in '@' for limited support of the password feature (unv://hunter2@1.2.3.4)
    QRegularExpression forbiddenChar(R"regex([^a-zA-Z0-9./\-\]_:@])regex");
    int forbiddenIndex = url.indexOf(forbiddenChar);
    if (forbiddenIndex >= 0) {
        argParseError("URL \"" + args[0] + "\" contains forbidden character '"
                      + url[forbiddenIndex] + "'");
    }

    return "unv://" + url;
}

CommandLineOptions getCommandLineOptions(const QApplication& app) {
    QCommandLineOption logFileNameOption("logfile");
    logFileNameOption.setValueName("filename");
    QCommandLineOption ariaLogFilenameOption("arialogfile");
    ariaLogFilenameOption.setValueName("filename");
    QCommandLineOption splashMsOption("splashms");
    splashMsOption.setValueName("duration in milliseconds");
    QCommandLineOption internalCommandOption("internalcommand");
    internalCommandOption.setValueName("command");
    QCommandLineParser optionParser;
    optionParser.addHelpOption();
    optionParser.addVersionOption();
    optionParser.addOption(logFileNameOption);
    optionParser.addOption(ariaLogFilenameOption);
    optionParser.addOption(splashMsOption);
    optionParser.addOption(internalCommandOption);
    optionParser.addPositionalArgument("URL", "address of Unvanquished server to connect to", "[URL]");
    optionParser.process(app);
    CommandLineOptions options;
    options.connectUrl = getURLFromPositionalOptions(optionParser);
    options.logFilename = optionParser.value(logFileNameOption);
    options.ariaLogFilename = optionParser.value(ariaLogFilenameOption);
    int splashMs = optionParser.value(splashMsOption).toInt();
    if (splashMs > 0) {
        options.splashMilliseconds = splashMs;
    }
    if (optionParser.isSet(internalCommandOption)) {
        QString command = optionParser.value(internalCommandOption);
        if (command == "playnow") {
            options.relaunchCommand = RelaunchCommand::PLAY_NOW;
        } else if (command == "updategame") {
            options.relaunchCommand = RelaunchCommand::UPDATE_GAME;
        } else if (command.startsWith("updateupdater:")) {
            options.relaunchCommand = RelaunchCommand::UPDATE_UPDATER;
            options.updateUpdaterVersion = command.section(':', 1);
        } else {
            argParseError("Invalid --internalcommand option: " + command);
        }
    }
    return options;
}

} // namespace

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("Unvanquished Updater");
    QCoreApplication::setApplicationVersion(GIT_VERSION);
    QCoreApplication::setOrganizationName("Unvanquished Development");
    QCoreApplication::setOrganizationDomain("unvanquished.net");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    // The font is already needed to display our arg parsing error on Linux
    int fontId = QFontDatabase::addApplicationFont(":resources/Roboto-Regular.ttf");
    if (fontId == -1) {
        qDebug() << "Failed to register Roboto font";
    } else {
        QFont font("Roboto");
        font.setPointSize(10);
        app.setFont(font);
    }

    CommandLineOptions options = getCommandLineOptions(app);
    if (!options.logFilename.isEmpty()) {
        logFile.setFileName(options.logFilename);
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qInstallMessageHandler(&LogMessageHandler);
        } else {
            qDebug() << "Failed to open log file for writing";
        }
    }

    if (!options.connectUrl.isEmpty()) {
        qDebug() << "Will connect to URL:" << options.connectUrl;
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

    Settings settings;
    GameLauncher gameLauncher(options.connectUrl, settings);

    if (options.relaunchCommand == RelaunchCommand::PLAY_NOW) {
        gameLauncher.startGame(/*useConnectUrl=*/ true, /*failIfWindowsAdmin=*/ true);
        return 0;
    }

    SplashController splashController(
        options.relaunchCommand, options.updateUpdaterVersion, options.connectUrl, settings);
    splashController.checkForUpdate();
    QmlDownloader downloader;
    downloader.ariaLogFilename_ = options.ariaLogFilename;
    downloader.connectUrl_ = options.connectUrl;
    QQmlApplicationEngine engine;
    engine.addImportPath(QLatin1String("qrc:/"));
    engine.addImageProvider(QLatin1String("fluidicons"), new IconsImageProvider());
    engine.addImageProvider(QLatin1String("fluidicontheme"), new IconThemeImageProvider());
    auto* context = engine.rootContext();
    context->setContextProperty("updaterSettings", &settings);
    context->setContextProperty("gameLauncher", &gameLauncher);
    context->setContextProperty("splashController", &splashController);
    context->setContextProperty("downloader", &downloader);
    context->setContextProperty("splashMilliseconds", options.splashMilliseconds);
    qmlRegisterType<QmlDownloader>("QmlDownloader", 1, 0, "QmlDownloader");

    engine.load(QUrl(QLatin1String("qrc:/splash.qml")));
    return app.exec();
}
