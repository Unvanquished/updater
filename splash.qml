import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Material 1.0

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
        updaterWindowLoader.active = true
        splash.hide();
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

    ActionButton {
        id: settingsAction
        scale: 0.35
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: {
            left: -20
            top: -20
        }

        iconName: "action/settings"
        Material.elevation: 0
        Material.background: Material.Teal
        onClicked: {
            showUpdater();
            timer.stop();
        }
    }

    Loader {
        id: updaterWindowLoader
        active: false
        source: "qrc:/main.qml"
    }
}
