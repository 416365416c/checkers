#include "aibrain.h"
#include <QDebug>
#include <QVariant>
#include <QVariantMap>
#include <QMap>
#include <QMutexLocker>
#include <cstdio>
#include <cstdlib>
#include <cstring>
const int INF = 1000000;

void moveDump(const Move&m);
void boardDump(GameState *gs);
int sign(int a)
{
    //couldn't find it in cmath?
    return a < 0 ? -1 : 1;
}

int heuristic(GameState* state, int player=2) //Counts pieces per player, kings are worth 2
{
    int go = state->gameOver();
    if (go)
        return go == player ? INF/2 : (go == 3 ? 0 : -INF/2);
    int p[2];//%2 places p2 in 0 and p1 in 1
    p[0] = p[1] = 0;
    for(int i=0; i<4*8; i++)
        if(state->board[i] > 0)
            p[state->board[i] % 2] += (state->board[i] + 1) / 2;
    return p[player%2] - p[(player+1)%2];
}

bool isCapture(const Move &m) {
    return (abs(m.x2 - m.x1) == 2);
}

GameState* makeState(GameState* baseState, Move* move, bool inplace = false)
{
    GameState* ret = baseState;
    if (!inplace)
        ret = new GameState(*baseState);
    int tmp = baseState->get(move->x1, move->y1);
    if (abs(move->x2 - move->x1) == 2) //Jump
        ret->set(move->x1 + sign(move->x2 - move->x1), move->y1 + sign(move->y2 - move->y1), 0);
    ret->set(move->x1, move->y1, 0);
    if (move->y2 == 0 && tmp == 1)
        tmp = 3;
    if (move->y2 == 7 && tmp == 2)
        tmp = 4;
    ret->set(move->x2, move->y2, tmp);
    ret->lastMoveX = move->x2;
    ret->lastMoveY = move->y2;
    return ret;
}

void moveMaker(QList<Move>* list, GameState* state, int x, int y)
{
    int piece = state->get(x,y);
    if (piece <= 0)
        return;
    int player = piece % 2;
    bool kinged = piece > 2;
    QList<QPoint> coords;
    if (kinged || player == 0) {
        coords << QPoint(x+1, y+1);
        coords << QPoint(x-1, y+1);
    }
    if (kinged || player == 1) {
        coords << QPoint(x+1, y-1);
        coords << QPoint(x-1, y-1);
    }

    foreach(QPoint c, coords) {
        Move ret;
        ret.x1 = x;
        ret.y1 = y;
        ret.x2 = c.x();
        ret.y2 = c.y();
        ret.value = -INF;
        int otherPiece = state->get(ret.x2,ret.y2);
        if (otherPiece < 0 || (otherPiece > 0 && (otherPiece % 2 == piece % 2))) //invalid move
            continue;
        if (otherPiece > 0) { //Possible jump
            QPoint c2(x + 2*(ret.x2 - ret.x1),y + 2*(ret.y2-ret.y1));
            int otherOtherPiece = state->get(c2.x(), c2.y());
            if (otherOtherPiece != 0)
                continue;
            ret.x2 = c2.x();
            ret.y2 = c2.y();
        }
        *list << ret;
    }
}

bool applyCaptureRule(QList<Move>* moves) //if moves contains a capture, all non-captures are removed
{
    bool hasCap = false;
    int i;
    for(i=0; i<moves->length(); i++) {
        if (isCapture(moves->at(i))) {
            hasCap = true;
            break;
        }
    }

    if(!hasCap)
        return false;
    for(i=moves->length() - 1; i >= 0; i--)
        if (!isCapture(moves->at(i)))
            moves->removeAt(i);
    return true;
}

GameState::GameState()
{
    lastMoveX = lastMoveY = -1;
    memset(&board, 0, 4*8*sizeof(int));
}

GameState::GameState(GameState &other)
{
    lastMoveX = other.lastMoveX;
    lastMoveY = other.lastMoveY;
    memcpy(&board, &(other.board), 4*8*sizeof(int));
}

void GameState::clone(GameState &other)
{
    lastMoveX = other.lastMoveX;
    lastMoveY = other.lastMoveY;
    memcpy(&board, &(other.board), 4*8*sizeof(int));
}

// -1 is invalid, 0 is empty, 1 is p1, 2 is p2, 3 is p1king, 4 is p2king
int GameState::get(int x, int y) {
    if (x < 0 || x >= 8 || y < 0 || y >= 8)//Off the edge
        return -1;
    if (x%2 == y%2)//Unused diagonal
        return -1;
    return board[(x/2) + (y*4)];
}

void GameState::set(int x, int y, int val) {
    if (x < 0 || x >= 8 || y < 0 || y >= 8)
        return;
    if (x%2 == y%2)
        return;
    board[(x/2) + (y*4)] = val;
}

int GameState::gameOver() {//Returns winner, 0 (still going) or 3 (stalemate)
    int p[2];//%2 places p2 in 0 and p1 in 1, so player is the other player in terms of p idx
    p[0] = p[1] = 0;
    for(int i=0; i<4*8; i++)
        p[board[i] % 2] += board[i] > 0;
    if (!p[0])
        return 1;
    if (!p[1])
        return 2;

    //Check for stalemate
    bool p1move = false;
    bool p2move = false;
    for(int x = 0; x<8; x++) {
        for(int y = 0; y<8; y++) {
            int c = get(x,y);
            if (c <= 0)
                continue;
            QList<Move> moves;
            moveMaker(&moves, this, x, y);
            if (!moves.isEmpty()) {
                if (c % 2) {
                    p1move = true;
                } else {
                    p2move = true;
                }
            }
            if (p1move && p2move)
                return 0;
        }
    }
    return 3;
}

//AI is always P2, so player==2 determines max/min-ing
/*  Current design has additional captures viewed as additional moves,
    last moving piece is stored in game state, and if you capture you
    don't swap the min/max choice for the next move.

    It would be more computationally efficient but less programmer efficient
    code to try and store moves as valid chains.
*/
Move minimax(GameState* state, int alpha, int beta, int depth, int player) {
    //Boundary Condition
    //boardDump(state);
    if(state->gameOver() || depth <= 0) {
        Move ret;
        ret.x1 = ret.x2 = ret.y1 = ret.y2 = -1;
        ret.value = heuristic(state);
        if (depth > 0 && abs(ret.value) > 100)
            ret.value += depth;//Reward faster games
        return ret;
    }
    //select valid moves
    QList<Move> moves;
    int otherPlayer = (player == 1 ? 2 : 1);
    if (state->get(state->lastMoveX, state->lastMoveY) % 2 == player % 2) //We're doing a second move, after a capture
        moveMaker(&moves, state, state->lastMoveX, state->lastMoveY);
    for(int x = 0; x<8; x++)
        for(int y = 0; y<8; y++)
            if(state->get(x,y) > 0 && (state->get(x,y) % 2) == player % 2)
                moveMaker(&moves, state, x, y);
    applyCaptureRule(&moves);
    //run moves through recursive simulator to get values, and keep best move
    GameState nextState;
    Move best;
    best.x1 = best.x2 = best.y1 = best.y2 = -2;
    best.value = -INF;
    if(player == 1)
        best.value = INF;//Minimizing this player
    foreach(Move m, moves) {
        nextState.clone(*state);
        makeState(&nextState, &m, true);

        bool keepsGoing = isCapture(m);//### Send moves through, to avoid duplicating that calculation?
        if (keepsGoing){
            QList<Move> nextMoves;
            moveMaker(&nextMoves, &nextState, nextState.lastMoveX, nextState.lastMoveY);
            keepsGoing = applyCaptureRule(&nextMoves);
        }

        int mValue = minimax(&nextState, alpha, beta, depth-1, keepsGoing ? player : otherPlayer).value;
        if (player == 1) {
            if(mValue < best.value) {
                best = m;
                beta = best.value = mValue;
                if(beta <= alpha)
                    return best;
            }
        } else {
            if(mValue > best.value) {
                best = m;
                alpha = best.value = mValue;
                if(beta <= alpha)
                    return best;
            }
        }
    }
    return best;
}

void moveDump(const Move &m) {
    qDebug() << "Move: " << QPoint(m.x1, m.y1) << "to" << QPoint(m.x2, m.y2);
}

void boardDump(GameState *gs) {
    qDebug() << "State Dump";
    qDebug() << "Game Over:" << gs->gameOver();
    qDebug() << "Value: " << heuristic(gs);
    qDebug() << "Board: ";
    for(int y=0; y<8; y++) {
        for(int x=0; x<8; x++) {
            int val = gs->get(x,y);
            if (val >= 0)
                printf("%d", val);
            else
                printf(" ");
        }
        printf("\n");
    }
}

AIBrain::AIBrain(QObject* parent)
    : QObject(parent), m_maxDepth(100), m_thread(new ThinkThread)
{
    connect(m_thread, SIGNAL(finished()),
            this, SLOT(finishMove()));
}


void AIBrain::thinkHard(QVariantList input) {
    //###Add an argument for which player?, currently only P2
    GameState initialState;
    for (int i=0; i<input.size(); i++){
        int x = input.at(i).value<QObject*>()->property("col").toInt();
        int y = input.at(i).value<QObject*>()->property("row").toInt();
        if (x >= 8 || y >= 8) {
            qDebug() << QLatin1String("Error: AIBrain::thinkHard only works for 8x8 boards");
            return;
        }
        int p = input.at(i).value<QObject*>()->property("player").toInt();
        if (p >= 2) {
            qDebug() << QLatin1String("AIBrain::thinkHard only works for 2 players");
            return;
        }
        p++;//JS is 0-based, we're 1-based
        if (input.at(i).value<QObject*>()->property("king").toBool())
            p += 2;
        if (input.at(i).value<QObject*>()->property("midmultijump").toBool()) {
            initialState.lastMoveX = x;
            initialState.lastMoveY = y;
        }
        initialState.set(x,y,p);
    }
    m_thinking = true;
    emit thinkingChanged();

    QMutexLocker(&(m_thread->aMutex));
    m_thread->startingState = new GameState(initialState);
    m_thread->depth = m_maxDepth;
    m_thread->start();
    //resume in finishMove()
}

void AIBrain::finishMove()
{
    QMutexLocker(&(m_thread->aMutex));
    m_move = m_thread->result;
    emit moveChanged();
    m_thinking = false;
    emit thinkingChanged();
}

ThinkThread::ThinkThread()
    : QThread(), startingState(0)
{
}

void ThinkThread::run()
{
    QMutexLocker(&(this->aMutex));
    if (!startingState)
        return;
    // ### Switch to returning multiple moves at once so that a double capture is 'one' move AI wise?
    Move move = minimax(startingState, -INF, INF, depth, 2); // ### Some way to advance event loop for anims? Or just multithread it?
    //moveDump(move);
    result["pieceRow"] = move.y1;
    result["pieceCol"] = move.x1;
    result["targetRow"] = move.y2;
    result["targetCol"] = move.x2;
    delete startingState;
    startingState = 0;
    // finished signal gets emitted about here
}
