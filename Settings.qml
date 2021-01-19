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
                text: selectedInstallPath
            }
            FluidMaterial.ActionButton {
                Material.elevation: 1
                iconName: "file/folder"
                onClicked: fileDialog.open()
                enabled: downloader.state === QmlDownloader.IDLE
                opacity: enabled ? 1 : 0.38
            }
            FluidControls.TitleLabel {
                text: "Command Line:"
                padding: 16
            }
            TextField {
                text: updaterSettings.commandLine
                onEditingFinished: updaterSettings.commandLine = this.text
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a folder"
        onAccepted: {
            var clipRegex = /^file:\/\//;
            // On Windows, apparently, there are three slashes (unless it's on a network drive).
            if (Qt.platform.os === "windows") {
                clipRegex = /^file:(\/\/\/)?/;
            }
            var url = fileDialog.fileUrl.toString();
            console.log("file URL from dialog: " + url);
            var path = url + '/Unvanquished';
            path = path.replace(clipRegex, '').replace(/\/\/Unvanquished$/, '/Unvanquished');
            if (Qt.platform.os === "windows") {
                path = path.replace(/\//g, '\\');
            }
            selectedInstallPath = path;
        }
        selectFolder: true
    }
}

