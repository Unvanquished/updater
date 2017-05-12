import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

ApplicationWindow {
    visible: true
    width: 1280
    height: 720
    title: qsTr("Unvanquished Updater")
    Material.primary: Material.LightBlue
    Material.accent: Material.Blue

    Image {
        source: "qrc:/resources/menu720p.png"
    }
    News {}
}
