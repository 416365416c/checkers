import QtQuick 1.1
import "checkers.js" as Logic
import App 1.0 as App

Item {
    id: canvas
    property int tileSize: 80
    property Item multijumper: null
    property bool gameOver: true
    property int winner: -1
    property bool acceptingInput: !gameOver && !aiDelay
    property bool aiDelay: false
    property bool p2ai: true
    property int curPlayer: 0
    property int redLeft: 12
    property int blackLeft: 12
    property string playerStr: curPlayer == 0 ? "Red" : "Black"
    property string turnStr: playerStr + "'s move"
    property string multiStr: playerStr + " is still jumping"
    property string gameOverStr: winner >= 0 ? (winner == 0 ? "Red wins!" : "Black wins!") : 'Press "New Game" to play again';
    property QtObject brain: App.AIBrain{
        depth: 12
        onMoveChanged: Logic.finishAiMove();
    }
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

    MyText {
        color: winner == 0 ? "red" : "black"
        anchors.fill: parent
        anchors.margins: -64
        text: "GAME OVER!"
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 128
        font.bold: true
        visible: canvas.gameOver == true
        z: 1
    }
    MyText {
        anchors.horizontalCenter: parent.horizontalCenter
        y: -64
        font.pixelSize: 32
        text: if (canvas.multijumper != null) {
            multiStr;
        } else if (canvas.gameOver) {
            gameOverStr;
        } else if (brain.thinking) {
            'AI is thinking, please be patient';
        } else {
            turnStr;
        }
        z: 1
    }
    Rectangle { //Should just be border, as tiles cover the inner square
        anchors.fill: parent
        anchors.margins: -20
        color: "#A79176"
    }
}
