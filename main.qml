import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0 as FluidControls
import Fluid.Material 1.0 as FluidMaterial
import "utils.js" as Utils

ApplicationWindow {
    id: root
    visible: true
    width: 1024
    height: 768
    title: qsTr("Unvanquished Updater")
    Material.theme: Material.Light
    Material.primary: Material.DarkBlue
    Material.accent: Material.Violet

    property string selectedInstallPath: updaterSettings.installPath

    Connections {
        target: downloader
        ignoreUnknownSignals: true
        onStatusMessage: {
            console.log("Download status: " + message);
            infoBar.open(message);
        }
        onFatalMessage: {
            console.log("Installation failed: " + message);
            errorPopup.errorDetail = message;
            errorPopup.open();
            root.alert(0);
        }
    }

    Image {
      width: parent.width
      height: parent.height
      source: "qrc:/resources/background.png"
    }

    Rectangle {
        anchors.bottom: header.top
        width: header.width
        height: 3
        color: "#2F1E1E"
    }

    Image {
        id: header
        width: parent.width
        height: logo.height * 2
        anchors {
            top: parent.top
            topMargin: parent.height * 0.1
        }
        source: "qrc:/resources/header.png"
    }
    Rectangle {
        anchors.top: header.bottom
        width: header.width
        height: 3
        color: "#2F1E1E"
    }

    Image {
        id: tyrant
        anchors {
            right: parent.right
            verticalCenter: header.verticalCenter
            verticalCenterOffset: height * 0.20
        }
        width: parent.width * 0.25
        fillMode: Image.PreserveAspectFit
        source: "qrc:/resources/tyrant.png"
    }
    Image {
        id: logo
        anchors {
            left: parent.left
            verticalCenter: header.verticalCenter
            leftMargin: 25
        }
        width: parent.width * 0.65

        fillMode: Image.PreserveAspectFit
        source: "qrc:/resources/logo.png"
    }

    FluidMaterial.ActionButton {
        id: settingsAction
        scale: 0.55
        anchors.top: parent.top
        anchors.right: parent.right
        iconName: "action/settings"
        Material.elevation: 1
        Material.background: Material.Teal
        onClicked: settingsBottomSheet.open()
    }
    News {
        id: newsContainer
    }
    DownloadInfo {
        id: downloadInfo
    }
    FluidMaterial.BottomSheet {
        id: settingsBottomSheet
        width: parent.width
        maxHeight: parent.height * 0.30
        height: maxHeight
        Settings {}
        FluidMaterial.ActionButton {
            scale: 0.55
            anchors.top: parent.top
            anchors.right: parent.right
            iconName: "navigation/cancel"
            Material.elevation: 1
            Material.background: Material.Red
            onClicked: settingsBottomSheet.close()
        }
    }
    FluidControls.InfoBar {
        id: infoBar
        duration: 3000
        onClicked: {
            this.hide();
        }
    }

    UpdateFailed {
        id: errorPopup
        failedOperation: 'Game installation'
    }
}
