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
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0 as FluidControls
import Fluid.Material 1.0 as FluidMaterial
import QmlDownloader 1.0
import "utils.js" as Utils

Item {
    width: parent.width * 0.65
    height: Math.min(parent.height * 0.1, 40)

    anchors {
        bottom: parent.bottom
        horizontalCenter: parent.horizontalCenter
        bottomMargin: 60
    }

    FluidControls.Card {
        anchors {
            right: parent.right
        }

        width: parent.width - downloadAction.width - 15
        height: parent.height

        Material.elevation: 2
        Material.theme: Material.Dark
        Material.background: "black"

        Item {

            width: parent.width
            height: parent.height

            FluidControls.BodyLabel {
                id: instruction

                anchors {
                    top: parent.top
                    left: parent.left
                    leftMargin: 7
                    topMargin: 9
                }

                visible: true
                font.pixelSize: 22
                text: "Press the button to download the game"
            }

            ProgressBar {
                anchors.top: parent.top
                width: parent.width
                from: 0.0
                to: 1.0
                indeterminate: false
                value: downloader.completedSize / downloader.totalSize
                Material.accent: Material.Teal
            }

            Row {
                id: downloadInfo

                anchors {
                    bottom: parent.bottom
                    margins: 7
                }

                visible: false

                FluidControls.BodyLabel {
                    width: 5
                }

                FluidControls.BodyLabel {
                    id: completedDownload
                    text: Utils.humanSize(downloader.completedSize)
                    font.pixelSize: 17
                }

                FluidControls.BodyLabel {
                    text: " / "
                    font.pixelSize: 17
                }

                FluidControls.BodyLabel {
                    id: totalDownload
                    text: Utils.humanSize(downloader.totalSize)
                    font.pixelSize: 17
                }

                FluidControls.BodyLabel {
                    width: 20
                }

                FluidControls.BodyLabel {
                    id: eta
                    text: Utils.humanTime(downloader.eta)
                    font.pixelSize: 17
                }

                FluidControls.BodyLabel {
                    width: 20
                }

                FluidControls.BodyLabel {
                    text: "DL: "
                    font.pixelSize: 17
                }

                FluidControls.BodyLabel {
                    id: downloadSpeed
                    text: Utils.humanSize(downloader.downloadSpeed)
                    font.pixelSize: 17
                }

                FluidControls.BodyLabel {
                    width: 20
                    text: "/s"
                    font.pixelSize: 17
                }

                FluidControls.BodyLabel {
                    text: "UL: "
                    font.pixelSize: 17
                }

                FluidControls.BodyLabel {
                    id: uploadSpeed
                    text: Utils.humanSize(downloader.uploadSpeed)
                    font.pixelSize: 17
                }

                FluidControls.BodyLabel {
                    text: "/s"
                    font.pixelSize: 17
                }
            }
        }
    }

    FluidMaterial.ActionButton {
        id: downloadAction

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            top: parent.top
            // or bottom: parent.bottom
        }

        height: 70
        width: 70

        iconName: "file/file_download"
        scale: 1.2

        Material.elevation: 1
        Material.background: Material.Teal

        onClicked: {
            if (downloader.state === QmlDownloader.COMPLETED) {
                root.close();
                gameLauncher.startGame(/*useConnectUrl=*/ false, /*failIfWindowsAdmin=*/ false);
                return;
            }
            downloader.toggleDownload(selectedInstallPath);
        }
    }

    Connections {
        target: downloader
        ignoreUnknownSignals: true

        onStateChanged: {
            downloadInfo.visible = state !== QmlDownloader.COMPLETED;
            if (state === QmlDownloader.DOWNLOADING) {
                downloadAction.iconName = "av/pause";
                instruction.visible = false;
            } else if (state === QmlDownloader.PAUSED) {
                downloadAction.iconName = "file/file_download";
                instruction.visible = false;
            } else if (state === QmlDownloader.COMPLETED) {
                downloadAction.iconName = "av/play_arrow";
                instruction.visible = true;
                instruction.text = "Press the button to play the game";
                root.alert(0);
            }
        }
    }
}

