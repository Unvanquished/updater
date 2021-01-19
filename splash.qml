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

    Item {
        focus: true
        Keys.onSpacePressed: {
            // Do the same as if timer runs out, but immediately
            if (timer.running) {
                timer.stop();
                settingsAction.enabled = false;
                splashController.autoLaunchOrUpdate();
            }
        }
    }

    Timer {
        id: timer
        interval: splashMilliseconds
        repeat: false
        running: true
        onTriggered: {
            settingsAction.enabled = false;
            splashController.autoLaunchOrUpdate();
        }
    }

    function showUpdater() {
        splashDownloaderConnections.enabled = false;
        updaterWindowLoader.active = true
        splash.hide();
    }

    Connections {
        target: splashController

        onUpdateNeeded: {  // game update only
            if (updateNeeded) {
                showUpdater()
            } else {
                splash.close();
                gameLauncher.startGame(/*useConnectUrl=*/ true, /*failIfWindowsAdmin=*/ false);
            }
        }

        onUpdaterUpdateNeeded: {
            splashController.autoLaunchOrUpdate();
        }

        onUpdaterUpdate: {
            downloader.startUpdaterUpdate(updaterUrl);
            updaterUpdateLabel.visible = true;

            // Now allow the window to go behind other windows in the z-order.
            if (Qt.platform.os === "windows") {
                // On Windows, additionally change the flags so that the app has an icon
                // in the task bar (but remains borderless). The hide/show is needed to make the
                // task bar icon appear.
                splash.flags = Qt.Window | Qt.FramelessWindowHint;
                splash.hide();
                splash.show();
            } else {
                // Don't set the same window flags as on Windows because it makes the splash screen
                // change position on GNOME.
                // On Mac, WindowStaysOnTopHint doesn't even do anything and there is always
                // a taskbar icon.
                splash.flags &= ~Qt.WindowStaysOnTopHint;
            }
        }
    }

    Connections {
        id: splashDownloaderConnections
        target: downloader

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
        id: updaterUpdateLabel
        Material.theme: Material.Dark
        text: "Updating launcher..."
        visible: false
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
            settingsAction.enabled = false;
            if (splashController.relaunchForSettings()) {
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
