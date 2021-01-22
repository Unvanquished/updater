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
        splashConnections.enabled = false;
        updaterWindowLoader.active = true
        splash.hide();
    }

    Connections {
        id: splashConnections
        target: downloader
        ignoreUnknownSignals: true

        onUpdateNeeded: {
            if (updateNeeded) {
                showUpdater()
            } else {
                splash.close();
                downloader.startGame();
            }
        }

        onFatalMessage: {
            updateFailed.errorDetail = message;
            updateFailedWindow.show();
            updateFailed.open();
            splash.hide();
        }
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

        anchors {
            top: parent.top
            right: parent.right

            margins: {
                top: 5
                right: 9
            }
        }

        height: 48
        width: 48

        iconName: "action/settings"
        scale: 1.35

        opacity: enabled ? 1 : 0.38

        Material.elevation: 0
        Material.background: Material.Teal

        onClicked: {
            timer.stop();
            if (downloader.relaunchForSettings()) {
                splash.close();
            } else {
                showUpdater();
            }
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

    ApplicationWindow {
        id: updateFailedWindow
        visible: false
        width: updateFailed.width
        height: updateFailed.height
        maximumWidth: updateFailed.width
        maximumHeight: updateFailed.height
        UpdateFailed {
            id: updateFailed
            failedOperation: 'Launcher self-update'
        }
    }
}
