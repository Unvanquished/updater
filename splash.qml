import QtQuick 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0

ApplicationWindow {
    visible: true
    id: splash
    width: 725
    height: 228
    flags: Qt.SplashScreen | Qt.WindowStaysOnTopHint

    Timer {
        id: timer
        interval: 3000
        repeat: false
        running: true
        onTriggered: downloader.checkForUpdate()
    }

    function showUpdater() {
        var component = Qt.createComponent("main.qml");
        if (component.status === Component.Error) {
            console.log(component.errorString());
            return;
        }
        var window = component.createObject(splash);
        splash.hide();
        window.show();
    }

    Connections {
        target: downloader
        ignoreUnknownSignals: true
        onUpdateNeeded: {
            if (updateNeeded) {
                showUpdater()
            } else {
                splash.hide();
                downloader.startGame();
            }
        }
    }

    Image {
        width: parent.width
        height: parent.height
        source: "qrc:/resources/splash.png"
    }

    Image {
        id: settingsLauncher
        source: "qrc:/Fluid/Controls/action/settings.svg"
        width: 20
        height: 20
        anchors.bottom: parent.bottom

        MouseArea {
            anchors.fill: settingsLauncher
            onClicked: {
                showUpdater();
                timer.stop();
            }
        }

    }

    ColorOverlay {
        anchors.fill: settingsLauncher
        source: settingsLauncher
        color: "#ffffffff"
    }
}
