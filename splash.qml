import QtQuick 2.0
import QtQuick.Controls 2.0

ApplicationWindow {
    visible: true
    id: splash
    width: 725
    height: 228
    flags: Qt.SplashScreen | Qt.WindowStaysOnTopHint

    Image {
        width: parent.width
        height: parent.height
        source: "qrc:/resources/splash.png"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                var component = Qt.createComponent("main.qml")
                if (component.status === Component.Error) {
                    console.log(component.errorString())
                }
                var window = component.createObject(splash)
                window.show()
                splash.hide()
            }
        }
    }

}
