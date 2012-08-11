import QtQuick 1.1

Rectangle {
    property int tileSize: 80
    property int col: 0
    property int row: 0
    width: tileSize
    height: tileSize
    x: tileSize * col
    y: tileSize * row
    property color mainCol: (col + row) % 2 ? "#DEB887" : "#90652C"
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
