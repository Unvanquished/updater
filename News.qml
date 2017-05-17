import QtQuick 2.0
import QtQuick.Controls 2.1
import Fluid.Controls 1.0

Card {
    id: newsContainer
    width: Math.min(parent.width * 0.5, 400)
    height: width * 1.1
    anchors.right: parent.right
    anchors.verticalCenter: parent.verticalCenter
    anchors.margins: parent.width * 0.1

    function fetchNews() {
        console.log('fetching...');
        var news = new XMLHttpRequest();
        news.onreadystatechange = function() {
            if (news.readyState === XMLHttpRequest.DONE) {
                var newsObj = JSON.parse(news.responseText);
                var component = Qt.createComponent("qrc:/NewsCard.qml");
                for (var i = 0; i < newsObj['posts'].length; ++i) {
                    var object = component.createObject(swipe);
                    var post = newsObj['posts'][i];
                    if (post['attachments'] && post['attachments'].length > 0) {
                        object.source = Qt.resolvedUrl(post['attachments'][0]['images']['medium']['url']);
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
        width: parent.width * 0.9
        clip: true
        anchors.centerIn: parent
        anchors.horizontalCenter: newsContainer.horizontalCenter
        Component.onCompleted: fetchNews()
    }
    PageIndicator {
        id: indicator
        count: swipe.count
        currentIndex: swipe.currentIndex

        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
