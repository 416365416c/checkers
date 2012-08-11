.pragma library //Shared game state
var maxC = 8;
var maxR = 8;
var gameCanvas = null;
//Should store a fixed tilesize on gamestart?
var gameSquares = new Array(maxR*maxC);
var gamePieces = new Array;
var squareComponent = Qt.createComponent("GameSquare.qml");
var pieceComponent = Qt.createComponent("GamePiece.qml");
var captureMoves = new Array;
var otherMoves = new Array;
var animDur = 500;
var moveHighlightColor = "#A7A8CD"
var capHighlightColor = "#A0CCBB"
function idx2D(r, c) {
    return r*maxC + c;
}

function init(canvasItem) {
    gameCanvas = canvasItem;
    if (squareComponent.status == 3) // Is .import a QQ2 feature?
        console.log("Square error: " + squareComponent.errorString());
    if (pieceComponent.status == 3)
        console.log("Piece error: " + pieceComponent.errorString());
    for(var i = 0; i<maxR; i++)
        for(var j = 0; j<maxC; j++)
            gameSquares[idx2D(i,j)] = squareComponent.createObject(canvasItem,
                    {"row":i, "col":j, "tileSize":canvasItem.tileSize});
    console.log("Initialization performed.");
}

function newGame() {
    gameCanvas.gameOver = false
    for (var i = 0; i<gamePieces.length; i++)
        if(gamePieces[i] != null)
            gamePieces[i].destroy();
    gamePieces = new Array(24);
    var row = 0;
    var col = 1;
    for (var i = 0; i<12; i++) {
        gamePieces[i] = pieceComponent.createObject(gameCanvas,
                {"row": row, "col": col, "player":1, "canvas":gameCanvas});
        col += 2;
        if (col >= maxC) {
            col -= maxC;
            //staggered line effect
            col -= 1;
            if (col < 0) 
                col += 2;
            row += 1;
        }
    }

    row = maxR - 1;
    col = 0;
    for (var i = 0; i<12; i++) {
        gamePieces[i+12] = pieceComponent.createObject(gameCanvas,
                {"row": row, "col": col, "player":0, "canvas":gameCanvas});
        col += 2;
        if (col >= maxC) {
            col -= maxC;
            //staggered line effect
            col -= 1;
            if (col < 0) 
                col += 2;
            row -= 1;
        }
    }

    calcMoves(0);//Start first turn
}

function boardContents(row, col) {
    if(row < 0 || row >= maxR || col < 0 || col >= maxC)
        return "outside"
    if(!((row + col) % 2))
        return "unused"
    //TODO: assign pieces to squares for faster lookups
    for(var i=0; i<gamePieces.length; i++)
        if (gamePieces[i] != null && gamePieces[i].row == row && gamePieces[i].col == col)
            return gamePieces[i]
    return "empty"
}

function evaluateMoves(piece, row, col) {
    var content = boardContents(row, col);
    var otherPlayer = piece.player ? 0 : 1;
    if (content == "empty")
        otherMoves.push({"piece" : piece, "row" : row, "col" : col});
    else if (content.player == otherPlayer) {
        var dx = row - piece.row;
        var dy = col - piece.col;
        if (boardContents(row + dx, col + dy) == "empty")
            captureMoves.push({"piece" : piece, "row" : row + dx, "col" : col + dy, "target": content});
    }
}

function calcMoves(player) {
    captureMoves.splice(0,captureMoves.length);
    otherMoves.splice(0,otherMoves.length);
    for(var i=0; i<gamePieces.length; i++) {
        if(gamePieces[i] != null) {
            if(gamePieces[i].player == player) {
                var piece = gamePieces[i];
                var targetRow = player ? piece.row + 1 : piece.row - 1;
                evaluateMoves(piece, targetRow, piece.col + 1);
                evaluateMoves(piece, targetRow, piece.col - 1);
            }
        }
    }
    if (player == 0) //Help Humans with Highlights
        loadHighlights();
    victoryCheck();
}

function victoryCheck()
{
    var gameOver = false;
    if (otherMoves == 0 && captureMoves == 0)
        gameOver = true;
    else {
        var left = {"0":0,"1":0};
        for(var i=0; i<gamePieces.length; i++)
            left[gamePieces[i].player]++;
        if(left[0] + left[1] == 0)
            gameOver = true;
    }
    if (gameOver) {
        gameCanvas.gameOver = true;
    }
}

function executeMove(move, isJump)
{
    if (move.piece.player == 1)
        move.piece.aiMoving = true;
    move.piece.x = move.col * gameCanvas.tileSize;
    move.piece.y = move.row * gameCanvas.tileSize;
    move.piece.row = move.row;
    move.piece.col = move.col;
    move.piece.aiMoving = false;
    if (isJump) {
        for(var i=0; i<gamePieces.length; i++)
            if(gamePieces[i] == move.target)
                gamePieces.splice(i,1);
        move.target.pleaseDestroy();
        calcMoves(move.piece.player)//Any more jumps?
        if (captureMoves.length) {
            for(var i=0; i < captureMoves.length; i++) {
                if(captureMoves[i].piece == move.piece) {
                    gameCanvas.multijumper = move.piece;
                    if(move.piece.player == 1)
                        gameCanvas.delayedAiMove();//Delay so you see all jumps
                    return;
                }
            }
        }
    }
    gameCanvas.multijumper = null;
    var nextPlayer = move.piece.player ? 0 : 1;
    calcMoves(nextPlayer)
    if(nextPlayer == 1 && !gameCanvas.gameOver)
        AiMove();
}

function startMove(item)
{
    //Prune Highlights, optimized for loc not speed
    loadHighlights(item);//Clears others
}

var slipFactor = 16;
function finishMove(item)
{
    clearHighlights();
    if (gameCanvas.multijumper != null && gameCanvas.multijumper != item) {
        //Not valid
        item.x = item.col * gameCanvas.tileSize; //Drag overrode binding
        item.y = item.row * gameCanvas.tileSize; //Drag overrode binding
        loadHighlights();
        return;
    }
    var newRow = Math.floor((item.y + slipFactor) / gameCanvas.tileSize);
    var newCol = Math.floor((item.x + slipFactor) / gameCanvas.tileSize);
    if (captureMoves.length) {
        for(var i=0; i < captureMoves.length; i++) {
            if(captureMoves[i].piece == item && captureMoves[i].row == newRow && captureMoves[i].col == newCol) {
                executeMove(captureMoves[i], true);
                return;
            }
        }
    } else {
        for(var i=0; i < otherMoves.length; i++) {
            if(otherMoves[i].piece == item && otherMoves[i].row == newRow && otherMoves[i].col == newCol) {
                executeMove(otherMoves[i], false);
                return;
            }
        }
    }
    //Not valid
    item.x = item.col * gameCanvas.tileSize; //Drag overrode binding
    item.y = item.row * gameCanvas.tileSize; //Drag overrode binding
    loadHighlights();
}

function loadHighlights(item)
{
    clearHighlights(); //Expedient
    if (captureMoves.length) {
        for (var i =0; i<captureMoves.length; i++)
            if (item == undefined || captureMoves[i].piece == item)
                gameSquares[idx2D(captureMoves[i].row, captureMoves[i].col)].highlightColor = capHighlightColor;
    } else {
        for (var i =0; i<otherMoves.length; i++)
            if (item == undefined || otherMoves[i].piece == item)
                gameSquares[idx2D(otherMoves[i].row, otherMoves[i].col)].highlightColor = moveHighlightColor;
    }
}
function clearHighlights()
{
    for(var i = 0; i < gameSquares.length; i++)
        gameSquares[i].highlightColor = "black";
}

function AiMove()
{
    if (gameCanvas.multijumper != null) {
        for(var i=0; i < captureMoves.length; i++) {
            if(captureMoves[i].piece == gameCanvas.multijumper) {
                executeMove(captureMoves[i], true);
                return;
            }
        }
    }

    if (captureMoves.length) {
        var i = Math.floor(Math.random() * captureMoves.length);
        executeMove(captureMoves[i], true);
    } else {
        var i = Math.floor(Math.random() * otherMoves.length);
        executeMove(otherMoves[i], false);
    }
}
