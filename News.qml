import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0
import Fluid.Material 1.0

Card {
    id: newsContainer
    width: Math.min(parent.width * 0.5, 400)
    height: width * 1.1
    anchors.right: parent.right
    anchors.verticalCenter: parent.verticalCenter
    anchors.margins: parent.width * 0.1

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
                        object.source = Qt.resolvedUrl(post['thumbnail_images']['medium']['url']);
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
        height: parent.height * 0.9
        width: parent.width
        clip: true
        anchors.horizontalCenter: parent.horizontalCenter
        Component.onCompleted: fetchNews()
    }
    PageIndicator {
        id: indicator
        count: swipe.count
        currentIndex: swipe.currentIndex
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
    ActionButton {
        anchors.left: swipe.left
        anchors.verticalCenter: swipe.verticalCenter
        anchors.leftMargin: -width / 2
        Material.background: Material.Blue
        scale: 0.5
        iconName: "navigation/chevron_left"
        onClicked: swipe.decrementCurrentIndex()
    }
    ActionButton {
        anchors.right: swipe.right
        anchors.verticalCenter: swipe.verticalCenter
        anchors.rightMargin: -width / 2
        Material.background: Material.Blue
        scale: 0.5
        iconName: "navigation/chevron_right"
        onClicked: swipe.incrementCurrentIndex()
    }
}
