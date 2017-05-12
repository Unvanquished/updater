import QtQuick 2.0
import QtQuick.Controls 2.1


Pane {
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
                    console.log(post['attachments'][0]['images']['medium']['url']);
                    for (var key in object) console.log(key);
                    console.log(object.picture);
                    //object.children[0].children[0].source = Qt.resolvedUrl(post['attachments'][0]['images']['medium']['url']);
                    object.children[0].children[1].children[0].text = post['title_plain'];
                    object.children[0].children[1].children[1].text = post['excerpt'];
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
    }
    PageIndicator {
        id: indicator

        count: swipe.count
        currentIndex: swipe.currentIndex

        anchors.bottom: swipe.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
