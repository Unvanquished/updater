import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0
import Fluid.Effects 1.0
import QtQuick.Layouts 1.3

Flickable {
    id: item
    property url fullThumbSrc : ""
    property url smallThumbSrc : ""
    property string cardTitle : ""
    property string summary : ""
    property string url: ""

    Rectangle {
        id: rectangle

        anchors {
            left: parent.left
            top: parent.top
            margins: 20
        }

        width: parent.width/2 - 30
        height: parent.height - 50
        clip: true

        color: "transparent"

        Image {
            id: fallback

            anchors {
                horizontalCenter: parent.horizontalCenter
                verticalCenter: parent.verticalCenter
            }

            width: parent.width
            height: parent.height
            fillMode: Image.PreserveAspectCrop

            source: "qrc:/resources/unvanquished.png"

            visible: fullThumb.status === Image.Error && smallThumb.status === Image.Error
        }

        Image {
            id: smallThumb

            anchors {
                horizontalCenter: parent.horizontalCenter
                verticalCenter: parent.verticalCenter
            }

            width: parent.width
            height: parent.height
            fillMode: Image.PreserveAspectCrop

            source: item.smallThumbSrc

            visible: fullThumb.status !== Image.Ready
        }

        Image {
            id: fullThumb

            anchors {
                horizontalCenter: parent.horizontalCenter
                verticalCenter: parent.verticalCenter
            }

            width: parent.width
            height: parent.height
            fillMode: Image.PreserveAspectCrop

            source: item.fullThumbSrc
        }

        MouseArea {
            anchors.fill: parent

            onClicked: {
                Qt.openUrlExternally(url);
            }

            cursorShape: Qt.PointingHandCursor
        }

        BusyIndicator {
            anchors.centerIn: parent
            visible: fullThumb.status !== Image.Ready && fullThumb.status !== Image.Error
                && smallThumb.status !== Image.Ready && smallThumb.status !== Image.Error
        }
    }

    Card {
        anchors {
            right: parent.right
            top: parent.top
            margins: 20
        }

        width: parent.width/2 - 30
        height: parent.height - 50

        Layout.maximumHeight: parent.height - 50
        Material.theme: Material.Dark
        Material.background: "black"

        Rectangle {
            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            width: parent.width - 15
            height: parent.height - 15

            clip: true
            color: "transparent"

            Column {
                id: column

                width: parent.width
                height: parent.height
                spacing: Units.smallSpacing * 2

                TitleLabel {
                    id: title
                    text: item.cardTitle
                    font.pixelSize: 30
                    font.bold: true
                }

                BodyLabel {
                    id: summary

                    width: parent.width

                    wrapMode: Text.WordWrap
                    text: item.summary
                    textFormat: Text.RichText
                    font.pixelSize: 21
                    lineHeight: 24
                    color: "lightgrey"

                    onLinkActivated:  {
                        Qt.openUrlExternally(link);
                    }

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.NoButton
                        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                    }
                }
            }
        }
    }
}
