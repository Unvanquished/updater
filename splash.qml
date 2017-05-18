import QtQuick 2.0
import QtQuick.Controls 2.0

ApplicationWindow {
    visible: true
    id: splash
    width: 725
    height: 228
    flags: Qt.SplashScreen | Qt.WindowStaysOnTopHint

    Component.onCompleted: {
        console.log('onCompleted....')
        downloader.checkForUpdate()
    }

    Connections {
        target: downloader
        ignoreUnknownSignals: true
        onUpdateNeeded: {
            console.log('onUpdateNeeded ' + updateNeeded)
            if (updateNeeded) {
                var component = Qt.createComponent("main.qml");
                if (component.status === Component.Error) {
                    console.log(component.errorString());
                }
                var window = component.createObject(splash);
                splash.hide();
                window.show();
            } else {
                splash.hide();
                downloader.startGame();
            }
        }
    }

    Image {
        width: parent.width
        height: parent.height
        source: "qrc:/resources/splash.png"
    }

}
