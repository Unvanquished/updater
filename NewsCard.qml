import QtQuick 2.0
import QtQuick.Controls 2.0
import Fluid.Controls 1.0
Item {
    Card {
        id: card
        anchors.centerIn: parent
        width: 400
        height: 400

        Image {
            id: picture
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            height: 200

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
            }

            BodyLabel {
                id: summary
                wrapMode: Text.WordWrap
                width: parent.width
            }
        }
    }
}
