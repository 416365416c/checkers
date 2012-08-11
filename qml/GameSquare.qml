import QtQuick 1.1

Rectangle {
    property int tileSize: 80
    property int col: 0
    property int row: 0
    width: tileSize
    height: tileSize
    x: tileSize * col
    y: tileSize * row
    color: (col + row) % 2 ? "palegoldenrod" : "darkgoldenrod"
}
