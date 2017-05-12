import QtQuick 2.0
import QtQuick.Controls 2.1

Item {
    width: parent.width
    height: parent.height
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
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
                }
            }
        }
        news.open('GET', 'http://www.unvanquished.net/?json=get_recent_posts');
        news.send();
    }
    SwipeView {
        id: swipe
        currentIndex: 1
        anchors.fill: parent
        Component.onCompleted: fetchNews()
//         Repeater {
//           model: 6
//           NewsCard {}
//         }
    }
    PageIndicator {
        id: indicator

        count: swipe.count
        currentIndex: swipe.currentIndex

        anchors.top: swipe.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
