import QtQuick 1.1

Item {
    id: root
    width: 640
    height: 480
    Rectangle {
        color: "blanchedalmond"
        anchors.fill: parent
        GameCanvas {
            id: canvas
            width: 320
            height: 320
            tileSize: 320/8
            anchors.centerIn: parent
            Component.onCompleted: canvas.newGame();
        }
        Button {
            height: 32
            width: 206
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.margins: 8
            caption: canvas.p2ai ? "Give P2 control to Humans" : "Give P2 control to AI"
            onClicked: canvas.p2ai = !canvas.p2ai
        }
        Row {
            height: 32
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 8
            spacing: 8

            Button {
                height: parent.height
                caption: "New Game"
                onClicked: canvas.newGame();
            }

            Button {
                height: parent.height
                caption: "Quit"
                onClicked: Qt.quit();
            }
        }
    }
}
