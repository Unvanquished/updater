import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0
import Fluid.Material 1.0
import "utils.js" as Utils

Item {
    width: parent.width * 0.75
    height: Math.min(parent.height * 0.1, 40)
    anchors.bottom: parent.bottom
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.margins: 40
    Card {
        width: parent.width - (downloadAction.width / 2)
        height: parent.height
        Material.elevation: 2
        Item {
            width: parent.width
            height: parent.height

            ProgressBar {
                anchors.top: parent.top
                width: parent.width
                from: 0.0
                to: 1.0
                indeterminate: false
                value: downloader.completedSize / downloader.totalSize
            }
            Row {
                id: downloadInfo
                anchors.bottom: parent.bottom
                visible: false
                BodyLabel {
                    width: 5
                }

                BodyLabel {
                    id: completedDownload
                    text: Utils.humanSize(downloader.completedSize)
                }
                BodyLabel {
                    text: " / "
                }
                BodyLabel {
                    id: totalDownload
                    text: Utils.humanSize(downloader.totalSize)
                }
                BodyLabel {
                    width: 20
                }
                BodyLabel {
                    id: eta
                    text: Utils.humanTime(downloader.eta)
                }
                BodyLabel {
                    width: 20
                }

                BodyLabel {
                    text: "DL: "
                }
                BodyLabel {
                    id: downloadSpeed
                    text: Utils.humanSize(downloader.downloadSpeed)
                }
                BodyLabel {
                    width: 20
                    text: "/s"
                }

                BodyLabel {
                    text: "UL: "
                }
                BodyLabel {
                    id: uploadSpeed
                    text: Utils.humanSize(downloader.uploadSpeed)
                }
                BodyLabel {
                    text: "/s"
                }
            }
        }
    }
    ActionButton {
        id: downloadAction
        property bool started: false
        iconName: "av/play_arrow"
        scale: 0.83
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        Material.background: Material.Red
        Material.elevation: 1
        onClicked: {
            downloadInfo.visible = true;
            if (this.iconName === "av/pause") {
                this.iconName = "av/play_arrow";
            } else {
                this.iconName = "av/pause";
            }
            this.started = !this.started;
            downloader.toggleDownload();
        }
    }
}

