import QtQuick 2.0
import QtQuick.Controls 2.1
import Fluid.Controls 1.0
import Fluid.Layouts 1.0

Item {
    width: parent.width * 0.75
    height: Math.min(parent.height * 0.1, 40)
    anchors.bottom: parent.bottom
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.margins: 40
    Card {
        width: parent.width
        height: parent.height
        ProgressBar {
            anchors.top: parent.top
            width: parent.width
            from: 0.0
            to: 1.0
            indeterminate: false

            SequentialAnimation on value {
                running: true
                loops: NumberAnimation.Infinite

                NumberAnimation {
                    from: 0.0
                    to: 1.0
                    duration: 3000
                }
            }
        }
    }
}

