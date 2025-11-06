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

import Fluid 2.0 as Fluid
import QmlDownloader
import QtQuick 
import QtQuick.Controls
import QtQuick.Controls.Material
import "utils.js" as Utils

Item {
    width: parent.width * 0.65
    height: Math.min(parent.height * 0.1, 40)

    anchors {
        bottom: parent.bottom
        horizontalCenter: parent.horizontalCenter
        bottomMargin: 60
    }

    Fluid.Card {
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

            Fluid.BodyLabel {
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

                Fluid.BodyLabel {
                    width: 5
                }

                Fluid.BodyLabel {
                    id: completedDownload
                    text: Utils.humanSize(downloader.completedSize)
                    font.pixelSize: 17
                }

                Fluid.BodyLabel {
                    text: " / "
                    font.pixelSize: 17
                }

                Fluid.BodyLabel {
                    id: totalDownload
                    text: Utils.humanSize(downloader.totalSize)
                    font.pixelSize: 17
                }

                Fluid.BodyLabel {
                    width: 20
                }

                Fluid.BodyLabel {
                    id: eta
                    text: Utils.humanTime(downloader.eta)
                    font.pixelSize: 17
                }

                Fluid.BodyLabel {
                    width: 20
                }

                Fluid.BodyLabel {
                    text: "DL: "
                    font.pixelSize: 17
                }

                Fluid.BodyLabel {
                    id: downloadSpeed
                    text: Utils.humanSize(downloader.downloadSpeed)
                    font.pixelSize: 17
                }

                Fluid.BodyLabel {
                    width: 20
                    text: "/s"
                    font.pixelSize: 17
                }

                Fluid.BodyLabel {
                    text: "UL: "
                    font.pixelSize: 17
                }

                Fluid.BodyLabel {
                    id: uploadSpeed
                    text: Utils.humanSize(downloader.uploadSpeed)
                    font.pixelSize: 17
                }

                Fluid.BodyLabel {
                    text: "/s"
                    font.pixelSize: 17
                }
            }
        }
    }

    Fluid.FloatingActionButton {
        id: downloadAction

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            top: parent.top
            // or bottom: parent.bottom
        }

        height: 70
        width: 70

        icon.source: Fluid.Utils.iconUrl("file/file_download")
        icon.color: "white"
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

        function onStateChanged(state) {
            downloadInfo.visible = state !== QmlDownloader.COMPLETED;
            if (state === QmlDownloader.DOWNLOADING) {
                downloadAction.icon.source = Fluid.Utils.iconUrl("av/pause");
                instruction.visible = false;
            } else if (state === QmlDownloader.PAUSED) {
                downloadAction.icon.source = Fluid.Utils.iconUrl("file/file_download");
                instruction.visible = false;
            } else if (state === QmlDownloader.COMPLETED) {
                downloadAction.icon.source = Fluid.Utils.iconUrl("av/play_arrow");
                instruction.visible = true;
                instruction.text = "Press the button to play the game";
                root.alert(0);
            }
        }
    }
}

