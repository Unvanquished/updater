import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0 as FluidControls
import Fluid.Material 1.0

ApplicationWindow {
    visible: true
    id: splash
    width: 745
    height: 228
    flags: Qt.SplashScreen | Qt.WindowStaysOnTopHint

    Timer {
        id: timer
        interval: splashMilliseconds
        repeat: false
        running: true
        onTriggered: {
            settingsAction.enabled = false;
            downloader.autoLaunchOrUpdate();
        }
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

        // TODO: respond to onFatalMessage
    }

    Image {
        width: parent.width
        height: parent.height
        source: "qrc:/resources/splash.png"
    }

    FluidControls.BodyLabel {
        Material.theme: Material.Dark
        text: "Updating launcher..."
        visible: downloader.totalSize > 0
        anchors.bottom: updaterUpdateProgress.top
        anchors.left: parent.left
        anchors.margins: { left: 20 }
        font.pixelSize: 17
    }

    ActionButton {
        id: settingsAction
        scale: 0.8
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: {
            left: -9
            top: -9
        }
        opacity: enabled ? 1 : 0.38
        iconName: "action/settings"
        Material.elevation: 0
        Material.background: Material.Teal
        onClicked: {
            showUpdater();
            timer.stop();
        }
    }

    ProgressBar {
        id: updaterUpdateProgress
        anchors.bottom: parent.bottom
        width: parent.width
        from: 0.0
        to: 1.0
        indeterminate: false
        value: downloader.completedSize / downloader.totalSize
        Material.accent: Material.Teal
    }

    Loader {
        id: updaterWindowLoader
        active: false
        source: "qrc:/main.qml"
    }
}
