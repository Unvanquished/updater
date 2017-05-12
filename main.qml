import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

ApplicationWindow {
    visible: true
    width: 1280
    height: 720
    title: qsTr("Unvanquished Updater")
    Material.theme: Material.Light
    Material.primary: Material.DarkBlue
    Material.accent: Material.Violet
    Image {
      width: parent.width
      height: parent.height
      source: "qrc:/resources/menu720p.png"
    }
    News {}
}
