import QtQuick 1.1
import "checkers.js" as Logic

Item {
    id: container
    property QtObject canvas: null //Some type coercion problem in JS?
    property int player: 0
    property int tileSize: canvas.tileSize
    property int col: 0
    property int row: 0
    property bool king: false
    property bool midmultijump: canvas.multijumper == container
    width: tileSize
    height: tileSize
    property bool aiMoving: false
    Behavior on x { enabled: aiMoving; NumberAnimation { duration: Logic.animDur } }
    Behavior on y { enabled: aiMoving; NumberAnimation { duration: Logic.animDur } }
    x: tileSize * col
    y: tileSize * row
    Rectangle {
        anchors.fill: parent
        color: player == 0 ? "#F00000" : "#101010"
        border.color: player == 0 ? "#A00000" : "#505050"
        border.width: midmultijump ? 4 : 1
        radius: width/2
    }
    Image {
        id:img
        anchors.centerIn: parent
        source: player == 0 ? "images/white_king.png" : "images/black_king.png"
        visible: container.king
    }
    onKingChanged: kingAnim.running = true
    ParallelAnimation {
        id: kingAnim
        running: false
        NumberAnimation { target: img; property: "opacity"; from: 0.0; to: 1.0; }
        NumberAnimation { target: img; property: "scale"; from: 0.0; to: 0.8; easing.type: Easing.OutBack; easing.overshoot: 4.8 }
    }
    MouseArea {
        anchors.fill: parent
        enabled: canvas.acceptingInput
        drag.axis: Drag.XandYAxis
        drag.target: container
        onPressed: Logic.startMove(container);
        onReleased: Logic.finishMove(container);
    }
    function pleaseDestroy() {
        if(player == 0)
            canvas.redLeft--;
        else
            canvas.blackLeft--;
        deathAnim.start();
    }
    SequentialAnimation {
        id: deathAnim
        running: false
        NumberAnimation { target: container; property: "opacity"; from: 1.0; to: 0.0; easing.type: Easing.InQuad }
        ScriptAction { script: container.destroy(); }
    }
    ParallelAnimation {
        id: spawnAnim
        running: true
        NumberAnimation { target: container; property: "opacity"; from: 0.0; to: 1.0; easing.type: Easing.InQuart }
        NumberAnimation { target: container; property: "scale"; from: 10.0; to: 1.0; easing.type: Easing.OutQuad }
    }
}
