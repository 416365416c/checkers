import QtQuick 1.1
import "checkers.js" as Logic

Item {
    id: container
    property QtObject canvas: null //Some type coercion problem in JS?
    property int player: 0
    property int tileSize: canvas.tileSize
    property int col: 0
    property int row: 0
    width: tileSize
    height: tileSize
    property bool aiMoving: false
    Behavior on x { enabled: aiMoving; NumberAnimation { duration: Logic.animDur } }
    Behavior on y { enabled: aiMoving; NumberAnimation { duration: Logic.animDur } }
    x: tileSize * col
    y: tileSize * row
    Rectangle {
        anchors.fill: parent
        color: player == 0 ? "red" : "black"
        radius: width/2
    }
    MouseArea {
        anchors.fill: parent
        enabled: canvas.acceptingInput
        drag.axis: Drag.XandYAxis
        drag.target: container
        onPressed: Logic.startMove(container);
        onReleased: Logic.finishMove(container);
    }
}
