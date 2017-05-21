import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0
import Fluid.Effects 1.0

Flickable {
    id: item
    property url source : ""
    property string cardTitle : ""
    property string summary : ""
    property string url: ""
    Card {
        id: card
        width: parent.width
        height: parent.height

        Image {
            id: picture
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
                margins: 20
            }

            height: parent.height * 0.5
            source: item.source
            BusyIndicator {
                anchors.centerIn: parent
                visible: picture.status !== Image.Ready
            }
        }

        Column {
            id: column
            anchors {
                left: parent.left
                top: picture.bottom
                right: parent.right
                margins: Units.smallSpacing * 2
            }
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
            }
        }
        Button {
            text: "Read More"
            onClicked: Qt.openUrlExternally(item.url)
        }
    }
}
