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

    function fetchNews() {
        var news = new XMLHttpRequest();
        news.onreadystatechange = function() {
            if (news.readyState === XMLHttpRequest.DONE) {
                var newsObj = JSON.parse(news.responseText);
                var component = Qt.createComponent("qrc:/NewsCard.qml");
                for (var i = 0; i < newsObj['posts'].length; ++i) {
                    var object = component.createObject(swipe);
                    var post = newsObj['posts'][i];
                    if (post['thumbnail_images']) {
                        if (post['thumbnail_images']['full']) {
                            object.source = Qt.resolvedUrl(post['thumbnail_images']['full']['url']);
                        }
                        else {
                            object.source = "qrc:/resources/unvanquished.png"
                        }
                    }
                    object.cardTitle = post['title_plain'];
                    object.summary = post['excerpt'];
                    object.url = post['url'];
                }
            }
        }
        news.open('GET', 'http://www.unvanquished.net/?json=get_recent_posts');
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

        Component.onCompleted: fetchNews()
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
    }

    ActionButton {
        id: rightButton

        anchors.right: parent.right
        anchors.verticalCenter: swipe.verticalCenter
        scale: 0.75

        Material.background: Material.Teal

        iconName: "navigation/chevron_right"
        onClicked: swipe.incrementCurrentIndex()
    }
}
