/* Unvanquished Updater
 * Copyright (C) Unvanquished Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import Fluid 2.0 as Fluid
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Dialogs
import QtQuick.Layouts
import QmlDownloader

Item {
    id: settingsItem
    width: parent.width
    Column {
        width: parent.width
        Pane {
            width: parent.width
            padding: 16
            Fluid.DisplayLabel {
                text: "Settings"
            }
        }
        Fluid.ThinDivider {}
        GridLayout {
            columns: 3
            rows: 2
            Fluid.TitleLabel {
                text: "Installation Directory:"
                padding: 16
            }
            Fluid.Subheader {
                text: selectedInstallPath
            }
            Fluid.FloatingActionButton {
                Material.elevation: 1
                icon.source: Fluid.Utils.iconUrl("file/folder")
                onClicked: fileDialog.open()
                enabled: downloader.state === QmlDownloader.IDLE
                opacity: enabled ? 1 : 0.38
            }
            Fluid.TitleLabel {
                text: "Command Line:"
                padding: 16
            }
            TextField {
                Layout.preferredWidth: 0.28 * settingsItem.width
                text: updaterSettings.commandLine
                onEditingFinished: updaterSettings.commandLine = this.text
            }
        }
    }

    FolderDialog {
        id: fileDialog
        title: "Please choose a folder"
        onAccepted: {
            var clipRegex = /^file:\/\//;
            // On Windows, apparently, there are three slashes (unless it's on a network drive).
            if (Qt.platform.os === "windows") {
                clipRegex = /^file:(\/\/\/)?/;
            }
            var url = fileDialog.selectedFolder.toString();
            console.log("file URL from dialog: " + url);
            var path = url + '/Unvanquished';
            path = path.replace(clipRegex, '').replace(/\/\/Unvanquished$/, '/Unvanquished');
            if (Qt.platform.os === "windows") {
                path = path.replace(/\//g, '\\');
            }
            selectedInstallPath = path;
        }
    }
}

