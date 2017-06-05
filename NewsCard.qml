import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0
import Fluid.Effects 1.0
import QtQuick.Layouts 1.3

Flickable {
    id: item
    property url source : ""
    property string cardTitle : ""
    property string summary : ""
    property string url: ""

    Image {
        id: picture
        anchors {
            left: parent.left
            top: parent.top
            margins: 20
            verticalCenter: parent.verticalCenter
        }

        width: parent.width * 0.45
        source: item.source
        fillMode: Image.PreserveAspectFit
        BusyIndicator {
            anchors.centerIn: parent
            visible: picture.status !== Image.Ready
        }
    }

    Card {
        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
        width: picture.paintedWidth
        height: picture.paintedHeight
        Layout.maximumHeight: picture.paintedHeight
        Material.theme: Material.Dark
        Material.background: "black"
        visible: picture.status === Image.Ready

        Column {
            id: column
            width: parent.width
            height: parent.height
            spacing: Units.smallSpacing * 2

            TitleLabel {
                id: title
                text: item.cardTitle
            }

            BodyLabel {
                id: summary
                wrapMode: Text.WordWrap
                width: parent.width
                text: item.summary
                textFormat: Text.RichText
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
