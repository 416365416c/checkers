import QtQuick 1.1

Item {
    id: root
    width: 640
    height: 480
    Rectangle {
        color: "blanchedalmond"
        anchors.fill: parent
        GameCanvas {
            width: 320
            height: 320
            tileSize: 320/8
            anchors.centerIn: parent
        }
    }
}
