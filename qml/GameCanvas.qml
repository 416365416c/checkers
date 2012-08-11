import QtQuick 1.1
import "checkers.js" as Logic

Item {
    id: canvas
    property int tileSize: 80
    property Item multijumper: null
    property bool gameOver: false
    Component.onCompleted: Logic.init(canvas)
    Text {
        anchors.centerIn: parent
        text: "GAME OVER!"
        font.pixelSize: 72
        color: "firebrick"
        visible: canvas.gameOver == true
        z: 1
    }
    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        y: -64
        font.pixelSize: 32
        color: "firebrick"
        text: "Keep jumping!"
        visible: canvas.multijumper != null
        z: 1
    }
    Rectangle { //Should just be border, as tiles cover the inner square
        anchors.fill: parent
        anchors.margins: -20
        color: "burlywood"
    }
}
