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

import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0
import Fluid.Effects 1.0
import QtQuick.Layouts 1.3

Flickable {
    id: item
    property url fullThumbSrc : ""
    property url smallThumbSrc : ""
    property string cardTitle : ""
    property string summary : ""
    property string url: ""

    Rectangle {
        id: rectangle

        anchors {
            left: parent.left
            top: parent.top
            margins: 20
        }

        width: parent.width/2 - 30
        height: parent.height - 50
        clip: true

        color: "transparent"

        Image {
            id: fallback

            anchors {
                horizontalCenter: parent.horizontalCenter
                verticalCenter: parent.verticalCenter
            }

            width: parent.width
            height: parent.height
            fillMode: Image.PreserveAspectCrop

            source: "qrc:/resources/unvanquished.png"

            visible: fullThumb.status === Image.Error && smallThumb.status === Image.Error
        }

        Image {
            id: smallThumb

            anchors {
                horizontalCenter: parent.horizontalCenter
                verticalCenter: parent.verticalCenter
            }

            width: parent.width
            height: parent.height
            fillMode: Image.PreserveAspectCrop

            source: item.smallThumbSrc

            visible: fullThumb.status !== Image.Ready
        }

        Image {
            id: fullThumb

            anchors {
                horizontalCenter: parent.horizontalCenter
                verticalCenter: parent.verticalCenter
            }

            width: parent.width
            height: parent.height
            fillMode: Image.PreserveAspectCrop

            source: item.fullThumbSrc
        }

        MouseArea {
            anchors.fill: parent

            onClicked: {
                Qt.openUrlExternally(url);
            }

            cursorShape: Qt.PointingHandCursor
        }

        BusyIndicator {
            anchors.centerIn: parent
            visible: fullThumb.status !== Image.Ready && fullThumb.status !== Image.Error
                && smallThumb.status !== Image.Ready && smallThumb.status !== Image.Error
        }
    }

    Card {
        anchors {
            right: parent.right
            top: parent.top
            margins: 20
        }

        width: parent.width/2 - 30
        height: parent.height - 50

        Layout.maximumHeight: parent.height - 50
        Material.theme: Material.Dark
        Material.background: "black"

        Rectangle {
            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            width: parent.width - 15
            height: parent.height - 15 - 30

            clip: true
            color: "transparent"

            Column {
                id: excerptColumn

                width: parent.width
                height: parent.height
                spacing: Units.smallSpacing * 2

                TitleLabel {
                    id: title

                    width: parent.width
                    text: item.cardTitle

                    wrapMode: Text.WordWrap
                    clip: true
                    font.pixelSize: 30
                    font.bold: true
                }

                BodyLabel {
                    id: summary

                    width: parent.width
                    wrapMode: Text.WordWrap

                    text: item.summary.substring(0, item.summary.search("<a"))
                    textFormat: Text.RichText

                    font.pixelSize: 21
                    lineHeight: 24
                    color: "lightgrey"
                }
            }
        }

        Rectangle {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
            }

            width: parent.width - 15
            height: 30

            clip: true
            color: "transparent"

            Column {
                id: linkColumn

                width: parent.width
                height: parent.height
                spacing: Units.smallSpacing * 2

                BodyLabel {
                    id: link

                    width: parent.width

                    text: "<p style='text-align: right;'><a style='color:#00B2B8;' href='" + item.url + "'>&rarr; Continue reading</a></p>"
                    textFormat: Text.RichText

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
            }
        }
    }
}
