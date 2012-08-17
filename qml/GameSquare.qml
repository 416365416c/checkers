import QtQuick 1.1

Rectangle {
    property QtObject canvas: null
    property int tileSize: canvas.tileSize
    property int col: 0
    property int row: 0
    width: tileSize
    height: tileSize
    x: tileSize * col
    y: tileSize * row
    property color mainCol: (col + row) % 2 ? "#90652C" : "#DEB887"
    property color hiCol: "black"
    color: mainCol
    SequentialAnimation on color{
        id: pulseAnim
        loops: -1
        //alwaysRunToEnd: true
        running: false
        ColorAnimation {
            from: mainCol
            to: hiCol
            duration: 320
        }
        ColorAnimation {
            from: hiCol
            to: mainCol
            duration: 320
        }
    }
    property string highlightColor: "black"
    Connections {
        target: canvas
        ignoreUnknownSignals: true
        onGameOverChanged: highlightColor = "black"
    }
    onHighlightColorChanged: {
        if (highlightColor == "black") {
            pulseAnim.running = false;
            color = mainCol;
        } else {
            hiCol = highlightColor;
            pulseAnim.running = true;
        }
    }
}
