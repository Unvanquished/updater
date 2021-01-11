import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0 as FluidControls
import "utils.js" as Utils

Popup {
    modal: true
    focus: true
    width: 650
    height: 400
    closePolicy: Popup.NoAutoClose
    anchors.centerIn: parent
    Material.theme: Material.Dark
    property string errorDetail
    property string failedOperation

    FluidControls.BodyLabel {
        id: errorPopupBody
        width: parent.width
        text: ('<h2> ' + failedOperation + ' failed. </h2> ' +
               '<h3>' + Utils.htmlEscape(errorDetail) + '</h3> <br/>' +
               '<p> If errors persist, go to the ' +
               '<a href="https://forums.unvanquished.net">forums</a>, ' +
               '<a href="https://unvanquished.net/chat">IRC</a>, or ' +
               '<a href="https://discord.gg/pHwf5K2">Discord</a> ' +
               'for support. </p>')
        textFormat: Text.StyledText
        linkColor: "#00B2B8"
        wrapMode: Text.WordWrap
        font.pixelSize: 21
        lineHeight: 24
        onLinkActivated: {
            Qt.openUrlExternally(link);
        }
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
        }
    }

    FluidControls.BodyLabel {
        text: errorPopupBody.hoveredLink
        anchors.bottom: parent.bottom
    }
}