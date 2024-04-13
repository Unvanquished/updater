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
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0
import Fluid.Material 1.0

Item {
    width: parent.width

    anchors {
        horizontalCenter: parent.horizontalCenter
        top: header.bottom
        bottom: downloadInfo.top
        topMargin: 10
        bottomMargin: 10
    }

    function fetchNews(jsonUrl) {
        var news = new XMLHttpRequest();

        console.log("fetching posts json: " + jsonUrl);
        news.open('GET', jsonUrl);

        news.onreadystatechange = function() {
            if (news.readyState === XMLHttpRequest.DONE) {
                var newsObj = null;

                if (news.status == 200) {
                    try {
                        newsObj = JSON.parse(news.responseText);
                    } catch (error) {
                        console.log("failed to parse posts json with error: " + error.name);
                        console.log("json parse error message: " + error.message);
                    }
                } else {
                    console.log("failed to fetch posts json with code: " + news.status);
                    console.log("XHR status: " + news.statusText);
                }

                if (newsObj === null) {
                    var fallbackJsonUrl = 'qrc:/resources/disconnected_posts.json';

                    if (jsonUrl !== fallbackJsonUrl) {
                        console.log("fetching fallback posts json");
                        fetchNews(fallbackJsonUrl);
                        return;
                    }
                }

                var component = Qt.createComponent("qrc:/NewsCard.qml");

                for (var i = 0; i < newsObj['posts'].length; ++i) {
                    var object = component.createObject(swipe);
                    var post = newsObj['posts'][i];
                    var found = false;

                    if (post['thumbnail_images']) {
                        if (post['thumbnail_images']['full']) {
                            object.fullThumbSrc = Qt.resolvedUrl(post['thumbnail_images']['full']['url']);
                            found = true;
                        }

                        if (post['thumbnail_images']['thumbnail']) {
                            object.smallThumbSrc = Qt.resolvedUrl(post['thumbnail_images']['thumbnail']['url']);
                            found = true;
                        }
                    }

                    if (!found) {
                        console.log("thumbnail not found, use fallback thumbnail");
                        object.fullThumbSrc = "qrc:/resources/unvanquished.png";
                    }

                    object.cardTitle = post['title_plain'];
                    object.summary = post['excerpt'];
                    object.url = post['url'];
                }
            }
        }

        news.send();
    }

    SwipeView {
        id: swipe

        clip: true

        anchors {
            top: parent.top
            bottom: parent.bottom
            left: leftButton.right
            right: rightButton.left
        }

        Component.onCompleted: {
            fetchNews('https://unvanquished.net/api/get_recent_posts/');
        }
    }

    PageIndicator {
        id: indicator

        count: swipe.count
        currentIndex: swipe.currentIndex
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        delegate: Rectangle {
            implicitWidth: 11
            implicitHeight: 11
            radius: width

            opacity: index === swipe.currentIndex ? 1 : 0.4
            color: "darkgrey"

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if(index !== swipe.currentIndex) {
                        swipe.setCurrentIndex(index);
                    }
                }
            }
        }
    }

    ActionButton {
        id: leftButton

        anchors.left: parent.left
        anchors.verticalCenter: swipe.verticalCenter
        scale: 0.75

        Material.background: Material.Teal

        iconName: "navigation/chevron_left"
        onClicked: swipe.decrementCurrentIndex()
        enabled: swipe.currentIndex > 0
        opacity: enabled ? 1 : 0.38
    }

    ActionButton {
        id: rightButton

        anchors.right: parent.right
        anchors.verticalCenter: swipe.verticalCenter
        scale: 0.75

        Material.background: Material.Teal

        iconName: "navigation/chevron_right"
        onClicked: swipe.incrementCurrentIndex()
        enabled: swipe.currentIndex + 1 < swipe.count
        opacity: enabled ? 1 : 0.38
    }
}
