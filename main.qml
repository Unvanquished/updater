import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0 as FluidControls
import Fluid.Material 1.0 as FluidMaterial

ApplicationWindow {
    id: root
    visible: true
    width: 700
    height: 600
    title: qsTr("Unvanquished Updater")
    Material.theme: Material.Light
    Material.primary: Material.DarkBlue
    Material.accent: Material.Violet

    Connections {
        target: downloader
        ignoreUnknownSignals: true
        onOnStatusMessage: infoBar.open(message)
    }

    Image {
      width: parent.width
      height: parent.height
      source: "qrc:/resources/menu.png"
    }
    FluidMaterial.ActionButton {
        id: settingsAction
        anchors.top: parent.top
        anchors.right: parent.right
        iconName: "action/settings"
        Material.elevation: 1
        Material.background: Material.Blue
        onClicked: settingsBottomSheet.open()
    }

    News {}
    DownloadInfo {}
    FluidMaterial.BottomSheet {
        id: settingsBottomSheet
        width: parent.width
        maxHeight: parent.height * 0.25
        height: maxHeight
        Settings {}
    }
    FluidControls.InfoBar {
        id: infoBar
        buttonText: "OK"
    }
}
