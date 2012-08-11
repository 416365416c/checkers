import QtQuick 1.1
import "checkers.js" as Logic

Item {
    id: canvas
    property int tileSize: 80
    property Item multijumper: null
    property bool gameOver: true
    property bool acceptingInput: !gameOver && !aiDelay
    property bool aiDelay: false
    function newGame() {
        aiTimer.running = false;
        Logic.newGame();
    }
    function delayedAiMove() {
        aiDelay = true;
        aiTimer.running = true;
    }
    Timer {
        id: aiTimer
        interval: Logic.animDur
        repeat: false
        running: false
        onTriggered: {
            aiDelay = false;
            Logic.AiMove();
        }
    }
    Component.onCompleted: Logic.init(canvas)

    Text {
        anchors.centerIn: parent
        text: "GAME OVER!"
        font.pixelSize: 72
        font.bold: true
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
