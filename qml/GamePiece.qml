import QtQuick 1.1
import "checkers.js" as Logic

Item {
    id: container
    property int player: 0
    property int tileSize: 80
    property int col: 0
    property int row: 0
    width: tileSize
    height: tileSize
    x: tileSize * col
    y: tileSize * row
    Rectangle {
        anchors.fill: parent
        color: player == 0 ? "red" : "black"
        radius: width/2
    }
    MouseArea {
        anchors.fill: parent
        drag.axis: Drag.XandYAxis
        drag.target: container
        onPressed: Logic.startMove(container);
        onReleased: Logic.finishMove(container);
    }
}
