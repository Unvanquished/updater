import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import Fluid.Controls 1.0 as FluidControls
import Fluid.Material 1.0 as FluidMaterial
import QmlDownloader 1.0

Item {
    id: settingsItem
    width: parent.width
    property string installPath: ""
    property string commandLine: ""
    Column {
        width: parent.width
        Pane {
            width: parent.width
            padding: 16
            FluidControls.DisplayLabel {
                text: "Settings"
            }
        }
        FluidControls.ThinDivider {}
        GridLayout {
            columns: 3
            rows: 2
            FluidControls.TitleLabel {
                text: "Installation Directory:"
                padding: 16
            }
            FluidControls.Subheader {
                text: updaterSettings.installPath
            }
            FluidMaterial.ActionButton {
                Material.elevation: 1
                iconName: "file/folder"
                onClicked: fileDialog.open()
                enabled: downloader.state === QmlDownloader.COMPLETED || downloader.state === QmlDownloader.IDLE
            }
            FluidControls.TitleLabel {
                text: "Command Line:"
                padding: 16
            }
            TextField {
                text: updaterSettings.commandLine
                onAccepted: updaterSettings.commandLine = this.text
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a folder"
        onAccepted: {
            var clipLength = 'file://'.length;
            // On Windows, apparently, there are three slashes. Increment
            // the substring clip length as a result.
            if (Qt.platform.os === "windows") {
                clipLength += 1;
            }
            updaterSettings.installPath = Qt.resolvedUrl(fileDialog.fileUrl).substring(clipLength);
        }
        selectFolder: true
    }
}

